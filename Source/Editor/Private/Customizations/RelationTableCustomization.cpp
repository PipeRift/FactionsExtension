// Copyright 2015-2023 Piperift. All Rights Reserved.

#include "Customizations/RelationTableCustomization.h"

#include "Customizations/SFaction.h"
#include "Faction.h"
#include "FactionsEditorStyle.h"
#include "FactionsSubsystem.h"

#include <DetailLayoutBuilder.h>
#include <DetailWidgetRow.h>
#include <Editor.h>
#include <IDetailChildrenBuilder.h>
#include <IDetailPropertyRow.h>
#include <ScopedTransaction.h>
#include <Styling/StyleColors.h>
#include <Widgets/Input/SButton.h>
#include <Widgets/Input/SSearchBox.h>
#include <Widgets/Layout/SScrollBox.h>


#define LOCTEXT_NAMESPACE "FRelationTableCustomization"


const FName FRelationTableCustomization::SourceId("Source");
const FName FRelationTableCustomization::TargetId("Target");
const FName FRelationTableCustomization::AttitudeId("Relation");
const FName FRelationTableCustomization::BidirectionalId("Bidirectional");
const FName FRelationTableCustomization::DeleteId("Delete");


class SRelationViewItem : public SMultiColumnTableRow<TSharedPtr<uint32>>
{
public:
	SLATE_BEGIN_ARGS(SRelationViewItem) {}
	/** The widget that owns the tree.  We'll only keep a weak reference to it. */
	SLATE_ARGUMENT(TSharedPtr<FRelationTableCustomization>, Customization)
	SLATE_ARGUMENT(uint32, Index)
	SLATE_END_ARGS()

	/** Construct function for this widget */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		Customization = InArgs._Customization;
		Index = InArgs._Index;

		SMultiColumnTableRow<TSharedPtr<uint32>>::Construct(
			FSuperRowType::FArguments().Style(FAppStyle::Get(), "DataTableEditor.CellListViewRow"),
			InOwnerTableView);
	}

	/** Overridden from SMultiColumnTableRow.  Generates a widget for this column of the list view. */
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& Name) override
	{
		TSharedPtr<FRelationTableCustomization> CustomizationPtr = Customization.Pin();

		return CustomizationPtr.IsValid() ? CustomizationPtr->MakeColumnWidget(Index, Name)
										  : SNullWidget::NullWidget;
	}

private:
	/** Weak reference to the database customization that owns our list */
	TWeakPtr<FRelationTableCustomization> Customization;

	uint32 Index;
};


TSharedRef<IPropertyTypeCustomization> FRelationTableCustomization::MakeInstance()
{
	return MakeShareable(new FRelationTableCustomization);
}

void FRelationTableCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;
	ListHandle = StructHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRelationTable, List));
	ListHandleArray = ListHandle->AsSet();

	// Refresh when Num changes
	OnItemsNumChanged = FSimpleDelegate::CreateRaw(this, &FRelationTableCustomization::RefreshRelations);
	ListHandleArray->SetOnNumElementsChanged(OnItemsNumChanged);

	RefreshRelations();

	auto AddButton = PropertyCustomizationHelpers::MakeAddButton(
		FSimpleDelegate::CreateSP(this, &FRelationTableCustomization::OnNewRelation),
		LOCTEXT("AddRelationToolTip", "Adds Relation"));

	auto ClearButton = PropertyCustomizationHelpers::MakeEmptyButton(
		FSimpleDelegate::CreateSP(this, &FRelationTableCustomization::OnClearRelations),
		LOCTEXT("ClearRelationToolTip", "Removes all relations"));

	HeaderRow.NameContent()
		.HAlign(HAlign_Fill)
			[SNew(SHorizontalBox) +
				SHorizontalBox::Slot().HAlign(HAlign_Left)[StructHandle->CreatePropertyNameWidget()] +
				SHorizontalBox::Slot()
					.HAlign(HAlign_Right)
					.AutoWidth()
					.Padding(4, 0)[SNew(SBox).MaxDesiredHeight(20.f).MinDesiredWidth(
						100.f)[SNew(SSearchBox)
								   .Visibility(this, &FRelationTableCustomization::GetFilterVisibility)
								   .OnTextChanged(this, &FRelationTableCustomization::OnFilterChanged)]]]
		.ValueContent()[SNew(SHorizontalBox) +
						SHorizontalBox::Slot()
							.VAlign(VAlign_Center)
							.AutoWidth()[SNew(STextBlock)
											 .Text(this, &FRelationTableCustomization::GetHeaderValueText)] +
						SHorizontalBox::Slot()
							.Padding(2.0f)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.AutoWidth()[AddButton] +
						SHorizontalBox::Slot()
							.Padding(2.0f)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.AutoWidth()[ClearButton]];

	GEditor->RegisterForUndo(this);
}

void FRelationTableCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
	IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedRef<SScrollBar> VerticalScrollBar =
		SNew(SScrollBar).Orientation(Orient_Vertical).Thickness(FVector2D(8.0f, 8.0f));

	TSharedRef<SHeaderRow> RelationsHeaderRow =
		SNew(SHeaderRow) +
		SHeaderRow::Column(DeleteId)
			.HAlignCell(HAlign_Fill)
			.VAlignCell(VAlign_Fill)
			.VAlignHeader(VAlign_Center)
			.FixedWidth(22.f)[SNew(STextBlock).Text(FText::GetEmpty())] +
		SHeaderRow::Column(SourceId)
			.HAlignCell(HAlign_Left)
			.ManualWidth(175.f)
			.HeaderContentPadding(FMargin(5, 2))[SNew(STextBlock).Text(FText::FromName(SourceId))] +
		SHeaderRow::Column(BidirectionalId)
			.HAlignCell(HAlign_Left)
			.ManualWidth(24.f)
			.HeaderContentPadding(FMargin(5, 2))[SNew(STextBlock).Text(FText{})] +
		SHeaderRow::Column(TargetId)
			.HAlignCell(HAlign_Left)
			.ManualWidth(175.f)
			.HeaderContentPadding(FMargin(5, 2))[SNew(STextBlock).Text(FText::FromName(TargetId))] +
		SHeaderRow::Column(AttitudeId)
			.HAlignCell(HAlign_Left)
			.FillWidth(1.f)
			.HeaderContentPadding(FMargin(5, 2))[SNew(STextBlock).Text(FText::FromName(AttitudeId))];


	RelationListView = SNew(SListView<TSharedPtr<uint32>>)
						   .ListItemsSource(&VisibleRelations)
						   .HeaderRow(RelationsHeaderRow)
						   .OnGenerateRow(this, &FRelationTableCustomization::MakeRelationWidget)
						   .OnListViewScrolled(this, &FRelationTableCustomization::OnRelationsScrolled)
						   .OnSelectionChanged(this, &FRelationTableCustomization::OnRelationSelected)
						   .ExternalScrollbar(VerticalScrollBar)
						   .ConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible)
						   .SelectionMode(ESelectionMode::None)
						   .AllowOverscroll(EAllowOverscroll::No);


	StructBuilder.AddCustomRow(LOCTEXT("RelationsPropertySearch", "Relations"))
		.ValueContent()
		.HAlign(HAlign_Fill)[SNew(SBox).Padding(FMargin{0, 0, 0,
			8})[SNew(SHorizontalBox) +
				SHorizontalBox::Slot().FillWidth(
					1.f)[SNew(SBox).MaxDesiredWidth(300.f)[RelationListView.ToSharedRef()]] +
				SHorizontalBox::Slot().AutoWidth()[SNew(SBox).WidthOverride(16.f)[VerticalScrollBar]]]];
}

FRelationTableCustomization::~FRelationTableCustomization()
{
	GEditor->UnregisterForUndo(this);
}


TSharedRef<SWidget> FRelationTableCustomization::CreateFactionWidget(
	TSharedRef<IPropertyHandle> PropertyHandle)
{
	auto IdHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFaction, Id));

	return SNew(SBox)
		.HAlign(HAlign_Fill)
		.Padding(FMargin{3, 0})
		.MinDesiredWidth(175.f)
		.MaxDesiredWidth(
			250.f)[SNew(SFaction)
					   .Faction_Lambda([IdHandle]() {
						   FName Id;
						   IdHandle->GetValue(Id);
						   return FFaction{Id};
					   })
					   .OnFactionSelected_Lambda([IdHandle](FFaction Faction, ESelectInfo::Type) {
						   IdHandle->SetValue(Faction.GetId());
					   })];
}

TSharedRef<ITableRow> FRelationTableCustomization::MakeRelationWidget(
	TSharedPtr<uint32> RelationIndex, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SRelationViewItem, OwnerTable).Customization(SharedThis(this)).Index(*RelationIndex);
}

TSharedRef<SWidget> FRelationTableCustomization::MakeColumnWidget(uint32 RelationIndex, FName ColumnName)
{
	uint32 Num;
	ListHandleArray->GetNumElements(Num);

	// Valid index?
	if (Num > RelationIndex)
	{
		const TSharedPtr<IPropertyHandle> RelationHandle = ListHandleArray->GetElement(RelationIndex);
		check(RelationHandle.IsValid());

		if (ColumnName == DeleteId)
		{
			return SNew(SButton)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.ContentPadding(FMargin{0, 1, 0, 0})
				.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
				.ForegroundColor(FAppStyle::GetSlateColor("DefaultForeground"))
				.OnClicked(this, &FRelationTableCustomization::OnDeleteRelation,
					RelationIndex)[SNew(STextBlock)
									   .Font(FAppStyle::Get().GetFontStyle("FontAwesome.10"))
									   .Text(FText::FromString(FString(TEXT("\xf057"))) /*fa-times-circle*/)];
		}
		else if (ColumnName == SourceId)
		{
			const TSharedPtr<IPropertyHandle> FactionHandle{
				RelationHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, Source))};

			return CreateFactionWidget(FactionHandle.ToSharedRef());
		}
		else if (ColumnName == TargetId)
		{
			const TSharedPtr<IPropertyHandle> FactionHandle{
				RelationHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, Target))};

			return CreateFactionWidget(FactionHandle.ToSharedRef());
		}
		else if (ColumnName == AttitudeId)
		{
			const TSharedPtr<IPropertyHandle> AttitudeHandle{
				RelationHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, Attitude))};

			return SNew(SBox)
				.HAlign(HAlign_Fill)
				.Padding(FMargin{3, 0})
				.MinDesiredWidth(100.f)[AttitudeHandle->CreatePropertyValueWidget()];
		}
		else if (ColumnName == BidirectionalId)
		{
			const TSharedPtr<IPropertyHandle> BidirectionalHandle{
				RelationHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, bBidirectional))};

			return SNew(SBox)
				.HAlign(HAlign_Center)
				.Padding(FMargin{
					3, 0})[SNew(SCheckBox)
							   .Style(FFactionsEditorStyle::Get(), "Relation.DirectionalCheckBox")
							   .IsChecked_Lambda([=]() {
								   bool bValue = false;
								   BidirectionalHandle->GetValue(bValue);
								   return bValue ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
							   })
							   .OnCheckStateChanged_Lambda([=](const ECheckBoxState NewState) {
								   BidirectionalHandle->SetValue(NewState == ECheckBoxState::Checked);
							   })
							   .ToolTipText(BidirectionalHandle->GetToolTipText())];
		}
	}

	return SNullWidget::NullWidget;
}

void FRelationTableCustomization::PostUndo(bool bSuccess)
{
	if (bSuccess && StructHandle.IsValid())
	{
		RefreshRelations();
	}
}

void FRelationTableCustomization::PostRedo(bool bSuccess)
{
	if (bSuccess && StructHandle.IsValid())
	{
		RefreshRelations();
	}
}

void FRelationTableCustomization::OnRelationsScrolled(double InScrollOffset) {}

void FRelationTableCustomization::OnRelationSelected(
	TSharedPtr<uint32> InNewSelection, ESelectInfo::Type InSelectInfo)
{}

void FRelationTableCustomization::RefreshRelations()
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

		FName SourceName;
		ItemProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, Source))
			->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFaction, Id))
			->GetValue(SourceName);

		FName TargetName;
		ItemProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, Target))
			->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFaction, Id))
			->GetValue(TargetName);

		TSharedPtr<uint32> Item{MakeShared<uint32>(I)};
		AvailableRelations.Add(Item);
		if (FilterText.IsEmpty() || SourceName.ToString().Contains(FilterText) ||
			TargetName.ToString().Contains(FilterText))
		{
			VisibleRelations.Add(Item);
		}
	}

	if (RelationListView.IsValid())
	{
		RelationListView->RequestListRefresh();
	}
}

void FRelationTableCustomization::OnFilterChanged(const FText& Text)
{
	FilterText = Text.ToString();
	RefreshRelations();
}

EVisibility FRelationTableCustomization::GetFilterVisibility() const
{
	return EVisibility::Visible;
}

void FRelationTableCustomization::OnNewRelation()
{
	const FScopedTransaction Transaction(LOCTEXT("Relation_New", "Added new relation"));
	GetOuter()->Modify();

	ListHandleArray->AddItem();
}

FReply FRelationTableCustomization::OnDeleteRelation(uint32 Index)
{
	const FScopedTransaction Transaction(LOCTEXT("Relation_DeleteRelation", "Deleted relation"));
	GetOuter()->Modify();

	ListHandleArray->DeleteItem(Index);

	return FReply::Handled();
}

void FRelationTableCustomization::OnClearRelations()
{
	const FScopedTransaction Transaction(LOCTEXT("Relation_ClearRelations", "Deleted all relations"));
	GetOuter()->Modify();

	ListHandleArray->Empty();
}

UObject* FRelationTableCustomization::GetOuter() const
{
	if (!StructHandle.IsValid())
		return nullptr;

	// Customization -> Relations -> Settings
	TArray<UObject*> Objects;
	StructHandle->GetOuterObjects(Objects);

	return Objects.Num() ? Objects[0] : nullptr;
}

FText FRelationTableCustomization::GetHeaderValueText() const
{
	uint32 Num;
	if (ListHandleArray->GetNumElements(Num) != FPropertyAccess::Success)
		return FText::GetEmpty();

	return FText::Format(LOCTEXT("ValueDescription", "{0} relations"), FText::AsNumber(Num));
}

#undef LOCTEXT_NAMESPACE
