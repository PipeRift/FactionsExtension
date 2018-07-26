// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "RelationDatabaseCustomization.h"

#include <Editor.h>
#include <ScopedTransaction.h>
#include <DetailWidgetRow.h>
#include <IDetailPropertyRow.h>
#include <IDetailChildrenBuilder.h>
#include <Widgets/Layout/SScrollBox.h>
#include <Widgets/Input/SSearchBox.h>
#include <Widgets/Input/SButton.h>

#include "SFaction.h"

#include "FactionsSettings.h"
#include "Faction.h"

#define LOCTEXT_NAMESPACE "FRelationDatabaseCustomization"


const FName FRelationDatabaseCustomization::FactionAId("Faction A");
const FName FRelationDatabaseCustomization::FactionBId("Faction B");
const FName FRelationDatabaseCustomization::AttitudeId("Reaction");
const FName FRelationDatabaseCustomization::DeleteId("Delete");

const FName FRelationDatabaseCustomization::NameMember("Name");


class SRelationViewItem : public SMultiColumnTableRow<TSharedPtr<uint32>>
{
public:
	SLATE_BEGIN_ARGS(SRelationViewItem) {}
	/** The widget that owns the tree.  We'll only keep a weak reference to it. */
	SLATE_ARGUMENT(TSharedPtr<FRelationDatabaseCustomization>, Customization)
	SLATE_ARGUMENT(uint32, Index)
	SLATE_END_ARGS()

	/** Construct function for this widget */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		Customization = InArgs._Customization;
		Index = InArgs._Index;

		SMultiColumnTableRow<TSharedPtr<uint32>>::Construct(
			FSuperRowType::FArguments()
			.Style(FEditorStyle::Get(), "DataTableEditor.CellListViewRow"),
			InOwnerTableView
		);
	}

	/** Overridden from SMultiColumnTableRow.  Generates a widget for this column of the list view. */
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& Name) override
	{
		TSharedPtr<FRelationDatabaseCustomization> CustomizationPtr = Customization.Pin();

		return CustomizationPtr.IsValid()? CustomizationPtr->MakeColumnWidget(Index, Name) : SNullWidget::NullWidget;
	}

private:
	/** Weak reference to the database customization that owns our list */
	TWeakPtr<FRelationDatabaseCustomization> Customization;

	uint32 Index;
};


TSharedRef<IPropertyTypeCustomization> FRelationDatabaseCustomization::MakeInstance()
{
	return MakeShareable(new FRelationDatabaseCustomization);
}

void FRelationDatabaseCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;
	ListHandle = StructHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRelationDatabase, ConfigList));
	ListHandleArray = ListHandle->AsArray();

	RefreshRelations();


	TSharedRef<SScrollBar> VerticalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Vertical)
		.Thickness(FVector2D(8.0f, 8.0f));

	TSharedRef<SHeaderRow> RelationsHeaderRow = SNew(SHeaderRow)
	+ SHeaderRow::Column(FactionAId)
	.HAlignCell(HAlign_Left)
	.FillWidth(1)
	.HeaderContentPadding(FMargin(0, 3))
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(2, 0)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Text(FText::FromName(FactionAId))
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.Padding(4, 0)
		[
			SNew(SBox)
			.MaxDesiredHeight(20.f)
			.MinDesiredWidth(100.f)
			[
				SNew(SSearchBox)
				.InitialText(FText::GetEmpty())
				.OnTextChanged(this, &FRelationDatabaseCustomization::OnFactionFilterChanged, FactionAId)
			]
		]
	]
	+ SHeaderRow::Column(FactionBId)
	.HAlignCell(HAlign_Left).FillWidth(1)
	.HeaderContentPadding(FMargin(0, 3))
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(2, 0)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Text(FText::FromName(FactionBId))
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.Padding(4, 0)
		[
			SNew(SBox)
			.MaxDesiredHeight(20.f)
			.MinDesiredWidth(100.f)
			[
				SNew(SSearchBox)
				.InitialText(FText::GetEmpty())
				.OnTextChanged(this, &FRelationDatabaseCustomization::OnFactionFilterChanged, FactionBId)
			]
		]
	]
	+ SHeaderRow::Column(AttitudeId)
	.HAlignCell(HAlign_Left).FillWidth(1.5)
	.HeaderContentPadding(FMargin(0, 3))
	[
		SNew(STextBlock)
		.Text(FText::FromName(AttitudeId))
	]
	+ SHeaderRow::Column(DeleteId)
	.HAlignCell(HAlign_Right)
	.ManualWidth(20.f)
	.HeaderContentPadding(FMargin(0, 3))
	[
		SNew(STextBlock)
		.Text(FText::GetEmpty())
	];


	RelationListView = SNew(SListView<TSharedPtr<uint32>>)
		.ListItemsSource(&VisibleRelations)
		.HeaderRow(RelationsHeaderRow)
		.OnGenerateRow(this, &FRelationDatabaseCustomization::MakeRelationWidget)
		.OnListViewScrolled(this, &FRelationDatabaseCustomization::OnRelationsScrolled)
		.OnSelectionChanged(this, &FRelationDatabaseCustomization::OnRelationSelected)
		.ExternalScrollbar(VerticalScrollBar)
		.ConsumeMouseWheel(EConsumeMouseWheel::Always)
		.SelectionMode(ESelectionMode::None)
		.AllowOverscroll(EAllowOverscroll::No);


	HeaderRow
	.WholeRowContent()
	.HAlign(HAlign_Fill)
	[
		SNew(SBox)
		.Padding(FMargin{ 0.f, 0.f, 0.f, 4.f })
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 10)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					SNew(STextBlock)
					.TextStyle(FEditorStyle::Get(), "LargeText")
					.Text(LOCTEXT("Relations_Title", "Relations"))
				]
				+ SHorizontalBox::Slot()
				.Padding(2.f, 0.f)
				.HAlign(HAlign_Right)
				.FillWidth(1.f)
				[
					SNew(SButton)
					.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
					.Text(LOCTEXT("Relations_New", "New"))
					.ButtonStyle(FEditorStyle::Get(), "FlatButton.Light")
					.OnClicked(this, &FRelationDatabaseCustomization::OnNewRelation)
				]
				+ SHorizontalBox::Slot()
				.Padding(2.f, 0.f)
				.HAlign(HAlign_Right)
				.AutoWidth()
				[
					SNew(SButton)
					.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
					.Text(LOCTEXT("Relations_Clear", "Clear"))
					.ButtonStyle(FEditorStyle::Get(), "FlatButton.Light")
					.OnClicked(this, &FRelationDatabaseCustomization::OnClearRelations)
				]
			]
			+ SVerticalBox::Slot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.f)
				[
					SNew(SBox)
					.HeightOverride(400.f)
					[
						RelationListView.ToSharedRef()
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(16.f)
					[
						VerticalScrollBar
					]
				]
			]
		]
	];

	GEditor->RegisterForUndo(this);
}

void FRelationDatabaseCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{}

FRelationDatabaseCustomization::~FRelationDatabaseCustomization()
{
	GEditor->UnregisterForUndo(this);
}


TSharedRef<SWidget> FRelationDatabaseCustomization::CreateFactionWidget(TSharedRef<IPropertyHandle> PropertyHandle)
{
	return SNew(SBox)
	.Padding(1)
	.HAlign(HAlign_Fill)
	.MinDesiredWidth(1000.f)
	[
		SNew(SFaction, PropertyHandle)
	];
}

TSharedRef<ITableRow> FRelationDatabaseCustomization::MakeRelationWidget(TSharedPtr<uint32> RelationIndex, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SRelationViewItem, OwnerTable)
		.Customization(SharedThis(this))
		.Index(*RelationIndex);
}

TSharedRef<SWidget> FRelationDatabaseCustomization::MakeColumnWidget(uint32 RelationIndex, FName ColumnName)
{
	uint32 Num;
	ListHandleArray->GetNumElements(Num);

	// Valid index?
	if (Num > RelationIndex)
	{
		const TSharedPtr<IPropertyHandle> RelationHandle = ListHandleArray->GetElement(RelationIndex);
		if (!RelationHandle.IsValid())
		{
			return SNullWidget::NullWidget;
		}

		TSharedPtr<SWidget> Widget{};
		if (ColumnName == FactionAId)
		{
			const TSharedPtr<IPropertyHandle> FactionHandle{ RelationHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, FactionA)) };
			if (FactionHandle.IsValid())
			{
				Widget = CreateFactionWidget(FactionHandle.ToSharedRef());
			}
		}
		else if (ColumnName == FactionBId)
		{
			const TSharedPtr<IPropertyHandle> FactionHandle{ RelationHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, FactionB)) };
			if (FactionHandle.IsValid())
			{
				Widget = CreateFactionWidget(FactionHandle.ToSharedRef());
			}
		}
		else if (ColumnName == AttitudeId)
		{
			const TSharedPtr<IPropertyHandle> AttitudeHandle{ RelationHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, Attitude)) };
			if (AttitudeHandle.IsValid())
			{
				Widget = SNew(SBox)
					.HAlign(HAlign_Fill)
					.MinDesiredWidth(100.f)
					[
						AttitudeHandle->CreatePropertyValueWidget()
					];
			}
		}

		if (Widget.IsValid())
		{
			return SNew(SBox)
				.Padding(0)
				.MinDesiredWidth(20.f)
				[
					Widget.ToSharedRef()
				];
		}
		else if (ColumnName == DeleteId)
		{
			return SNew(SBox)
				.Padding(1)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.MaxDesiredWidth(20.f)
				.MaxDesiredHeight(20.f)
				[
					SNew(SButton)
					.Text(LOCTEXT("Relations_Delete", "✖"))
					.ButtonStyle(FEditorStyle::Get(), "FlatButton.Light")
					.TextFlowDirection(ETextFlowDirection::Auto)
					.ContentPadding(2)
					.OnClicked(this, &FRelationDatabaseCustomization::OnDeleteRelation, RelationIndex)
				];
		}
	}

	return SNullWidget::NullWidget;
}

void FRelationDatabaseCustomization::PostUndo(bool bSuccess)
{
	if (bSuccess && StructHandle.IsValid())
	{
		RefreshRelations();
	}
}

void FRelationDatabaseCustomization::PostRedo(bool bSuccess)
{
	if (bSuccess && StructHandle.IsValid())
	{
		RefreshRelations();
	}
}

void FRelationDatabaseCustomization::OnRelationsScrolled(double InScrollOffset)
{

}

void FRelationDatabaseCustomization::OnRelationSelected(TSharedPtr<uint32> InNewSelection, ESelectInfo::Type InSelectInfo)
{

}

void FRelationDatabaseCustomization::RefreshRelations()
{
	AvailableRelations.Empty();
	VisibleRelations.Empty();

	if (!ListHandleArray.IsValid())
		return;

	uint32 Num;
	FPropertyAccess::Result Result = ListHandleArray->GetNumElements(Num);

	if (Result != FPropertyAccess::Success)
		return;

	for (uint32 I = 0; I < Num; ++I)
	{
		TSharedRef<IPropertyHandle> ItemProperty = ListHandleArray->GetElement(I);

		FName FactionAName;
		ItemProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, FactionA))
			->GetChildHandle(NameMember)->GetValue(FactionAName);

		FName FactionBName;
		ItemProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, FactionB))
			->GetChildHandle(NameMember)->GetValue(FactionBName);

		TSharedPtr<uint32> Item { MakeShared<uint32>(I) };
		AvailableRelations.Add(Item);
		if ((FilterFactionA.IsEmpty() || FactionAName.ToString().Contains(FilterFactionA)) && (FilterFactionB.IsEmpty() || FactionBName.ToString().Contains(FilterFactionB)))
		{
			VisibleRelations.Add(Item);
		}
	}

	if (RelationListView.IsValid())
	{
		RelationListView->RequestListRefresh();
	}
}

void FRelationDatabaseCustomization::OnFactionFilterChanged(const FText& Text, FName Faction)
{
	if (Faction == FactionAId)
		FilterFactionA = Text.ToString();
	else if (Faction == FactionBId)
		FilterFactionB = Text.ToString();

	RefreshRelations();
}

FReply FRelationDatabaseCustomization::OnNewRelation()
{
	const FScopedTransaction Transaction(LOCTEXT("Relation_New", "Added new relation"));
	GetOuter()->Modify();

	ListHandleArray->AddItem();
	RefreshRelations();

	return FReply::Handled();
}

FReply FRelationDatabaseCustomization::OnDeleteRelation(uint32 Index)
{
	const FScopedTransaction Transaction(LOCTEXT("Relation_DeleteRelation", "Deleted relation"));
	GetOuter()->Modify();

	ListHandleArray->DeleteItem(Index);
	RefreshRelations();

	return FReply::Handled();
}

FReply FRelationDatabaseCustomization::OnClearRelations()
{
	const FScopedTransaction Transaction(LOCTEXT("Relation_ClearRelations", "Deleted all relations"));
	GetOuter()->Modify();

	ListHandleArray->EmptyArray();
	RefreshRelations();

	return FReply::Handled();
}

UObject* FRelationDatabaseCustomization::GetOuter() const
{
	if (!StructHandle.IsValid())
		return nullptr;

	// Customization -> Relations -> Settings
	TArray<UObject*> Objects;
	StructHandle->GetOuterObjects(Objects);

	return Objects.Num() ? Objects[0] : nullptr;
}

#undef LOCTEXT_NAMESPACE
