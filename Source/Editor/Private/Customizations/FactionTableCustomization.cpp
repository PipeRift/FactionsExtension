// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "FactionTableCustomization.h"

#include <Editor.h>
#include <ScopedTransaction.h>
#include <Widgets/Layout/SScrollBox.h>
#include <Widgets/Input/SSearchBox.h>
#include <Widgets/Input/SButton.h>

#include "SFaction.h"

#include "FactionsSettings.h"
#include "FactionTable.h"
#include "Widgets/SFactionColor.h"


#define LOCTEXT_NAMESPACE "FFactionTableCustomization"

const FName FFactionTableCustomization::ColumnSelect("Select");
const FName FFactionTableCustomization::ColumnId("Id");
const FName FFactionTableCustomization::ColumnColor("Color");
const FName FFactionTableCustomization::ColumnDelete("Delete");


class SFactionViewItem : public SMultiColumnTableRow<FFactionViewItem>
{
public:

	SLATE_BEGIN_ARGS(SFactionViewItem) {}
		/** The widget that owns the tree. We'll only keep a weak reference to it. */
		SLATE_ARGUMENT(TSharedPtr<FFactionTableCustomization>, Customization)
		SLATE_ARGUMENT(FFactionViewItem, Item)
	SLATE_END_ARGS()


	/** Construct function for this widget */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		Customization = InArgs._Customization;
		Item = InArgs._Item;

		SMultiColumnTableRow<FFactionViewItem>::Construct(
			FSuperRowType::FArguments()
			.Style(FEditorStyle::Get(), "DataTableEditor.CellListViewRow"),
			InOwnerTableView
		);
	}

	/** Overridden from SMultiColumnTableRow.  Generates a widget for this column of the list view. */
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& Column) override
	{
		if (Column == FFactionTableCustomization::ColumnSelect)
		{
			return SNew(STextBlock).Text(LOCTEXT("FactionColumnSelect_Value", "⊙"));
		}
		else if (Column == FFactionTableCustomization::ColumnColor)
		{
			return SNew(SFactionColor, Item.GetColorProperty());
		}
		else if (Column == FFactionTableCustomization::ColumnDelete)
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
				.OnClicked(Customization.Pin().Get(), &FFactionTableCustomization::OnDeleteFaction, Faction)
				];
		}

		return SNew(SBox)
		.Padding(FMargin{ 5.0f, 3.f })
		.MinDesiredHeight(25.f)
		[
			SAssignNew(IdNameSwitcher, SWidgetSwitcher)
			+ SWidgetSwitcher::Slot()
			[
				SNew(SBox)
				.VAlign(VAlign_Center)
				.MinDesiredWidth(30.f)
				.MinDesiredHeight(18.f)
				[
					SNew(STextBlock)
					.Text(this, &SFactionViewItem::GetEditableNameAsText)
					.OnDoubleClicked(this, &SFactionViewItem::OnIdNameDoubleClicked)
				]
			]
			+ SWidgetSwitcher::Slot()
			[
				SNew(SBox)
				.VAlign(VAlign_Center)
				.MinDesiredWidth(30.f)
				[
					SAssignNew(IdNameTextBox, SEditableTextBox)
					.Text(this, &SFactionViewItem::GetEditableNameAsText)
					.ToolTipText(LOCTEXT("EditNameId_Tooltip", "Replace a faction's Id"))
					.RevertTextOnEscape(true)
					.ClearKeyboardFocusOnCommit(true)
					.OnTextCommitted(this, &SFactionViewItem::OnIdNameCommited)
				]
			]
		];
	}

private:

	FText GetEditableNameAsText() const { return FText::FromName(Item.GetName()); }

	FReply OnIdNameDoubleClicked();

	void OnIdNameCommited(const FText& InText, ETextCommit::Type InCommitType) const;


	TWeakPtr<FFactionTableCustomization> Customization;
	FFactionViewItem Item;

	TSharedPtr<SWidgetSwitcher> IdNameSwitcher;
	TSharedPtr<SEditableTextBox> IdNameTextBox;
};


FReply SFactionViewItem::OnIdNameDoubleClicked()
{
	FReply Reply = FReply::Unhandled();

	if (Item.IsValid())
	{
		IdNameSwitcher->SetActiveWidgetIndex(1);

		// Get path to editable widget
		FWidgetPath EditableWidgetPath;
		FSlateApplication::Get().GeneratePathToWidgetUnchecked(IdNameTextBox.ToSharedRef(), EditableWidgetPath);

		// Set keyboard focus directly
		FSlateApplication::Get().SetKeyboardFocus(EditableWidgetPath, EFocusCause::SetDirectly);

		Reply = FReply::Handled();
	}

	return Reply;
}

void SFactionViewItem::OnIdNameCommited(const FText& InText, ETextCommit::Type InCommitType) const
{
	if (Customization.IsValid())
	{
		Customization.Pin()->OnFactionIdChange(InText, InCommitType, Faction);
	}

	if (InCommitType == ETextCommit::OnEnter || InCommitType == ETextCommit::OnUserMovedFocus)
	{
		IdNameSwitcher->SetActiveWidgetIndex(0);
	}
}

TSharedRef<IPropertyTypeCustomization> FFactionTableCustomization::MakeInstance()
{
	return MakeShareable(new FFactionTableCustomization);
}

void FFactionTableCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;
	ItemsHandle = StructHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionTable, Items));

	if (!ItemsHandle->IsValidHandle())
		return;

	ItemsHandleMap = ItemsHandle->AsMap();

	RefreshView();

	TSharedRef<SScrollBar> VerticalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Vertical)
		.Thickness(FVector2D(8.0f, 8.0f));

	TSharedRef<SHeaderRow> ListHeaderRow = SNew(SHeaderRow)
	+ SHeaderRow::Column(ColumnSelect).HAlignCell(HAlign_Center)
	.VAlignCell(VAlign_Center)
	.VAlignHeader(VAlign_Center)
	.FixedWidth(20.f)
	[
		SNew(STextBlock).Text(LOCTEXT("FactionColumnSelect", ""))
	]
	+ SHeaderRow::Column(ColumnId)
	.HAlignCell(HAlign_Fill)
	.VAlignCell(VAlign_Center)
	.VAlignHeader(VAlign_Center)
	.FillWidth(1.f)
	.HeaderContentPadding(FMargin(0, 3))
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(2, 0)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("FactionColumnTitle", "Faction Id"))
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
				.OnTextChanged(this, &FFactionTableCustomization::OnFilterChanged)
			]
		]
	]
	+ SHeaderRow::Column(ColumnColor)
	.HAlignCell(HAlign_Right)
	.VAlignCell(VAlign_Center)
	.VAlignHeader(VAlign_Center)
	.FixedWidth(60.f)
	.HeaderContentPadding(FMargin(0, 3))
	[
		SNew(STextBlock).Text(LOCTEXT("FactionColumnColor", "Color"))
	]
	+ SHeaderRow::Column(ColumnDelete)
	.HAlignCell(HAlign_Right)
	.VAlignCell(VAlign_Center)
	.VAlignHeader(VAlign_Center)
	.FixedWidth(20.f)
	.HeaderContentPadding(FMargin(0, 3))
	[
		SNew(STextBlock).Text(LOCTEXT("FactionColumnDelete", ""))
	];


	ListView = SNew(SListView<FFactionViewItem>)
		.ListItemsSource(&VisibleFactions)
		.HeaderRow(ListHeaderRow)
		.OnGenerateRow(this, &FFactionTableCustomization::MakeFactionWidget)
		.OnListViewScrolled(this, &FFactionTableCustomization::OnScrolled)
		.OnSelectionChanged(this, &FFactionTableCustomization::OnSelected)
		.ExternalScrollbar(VerticalScrollBar)
		.ConsumeMouseWheel(EConsumeMouseWheel::Always)
		.SelectionMode(ESelectionMode::None)
		.AllowOverscroll(EAllowOverscroll::No);

	HeaderRow
	.NameContent()
	.VAlign(VAlign_Top)
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
				.Text(LOCTEXT("FactionsTitle", "Factions"))
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
				.OnClicked(this, &FFactionTableCustomization::OnNewFaction)
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
				.OnClicked(this, &FFactionTableCustomization::OnClearFactions)
			]
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			[
				SNew(SBox)
				.HeightOverride(280.f)
				[
					ListView.ToSharedRef()
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(14.f)
				[
					VerticalScrollBar
				]
			]
		]
	]
	.ValueContent()
	.HAlign(HAlign_Fill)
	[
		SNew(SBox)
		.Padding(FMargin{ 10, 30, 10, 10 })
		.HeightOverride(350.f)
		[
			SAssignNew(FactionInfoContainer, SVerticalBox)
		]
	];

	GEditor->RegisterForUndo(this);
}

void FFactionTableCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{}


FFactionTableCustomization::~FFactionTableCustomization()
{
	GEditor->UnregisterForUndo(this);
}

TSharedRef<SWidget> FFactionTableCustomization::CreateFactionWidget(TSharedRef<IPropertyHandle> PropertyHandle)
{
	return SNew(SBox)
	.Padding(1)
	.HAlign(HAlign_Fill)
	.MinDesiredWidth(1000.f)
	[
		SNew(SFaction, PropertyHandle)
	];
}

TSharedRef<ITableRow> FFactionTableCustomization:: MakeFactionWidget(const FFactionViewItem& Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SFactionViewItem, OwnerTable)
		.Customization(SharedThis(this))
		.Item(Item);
}

void FFactionTableCustomization::PostUndo(bool bSuccess)
{
	if (bSuccess && StructHandle.IsValid())
	{
		RefreshView();
	}
}

void FFactionTableCustomization::PostRedo(bool bSuccess)
{
	if (bSuccess && StructHandle.IsValid())
	{
		RefreshView();
	}
}

void FFactionTableCustomization::OnScrolled(double InScrollOffset)
{

}

void FFactionTableCustomization::OnSelected(TSharedPtr<FFactionViewItem> InNewSelection, ESelectInfo::Type InSelectInfo)
{

}

void FFactionTableCustomization::SetSelection(const FFactionViewItem& InNewSelection, ESelectInfo::Type InSelectInfo /*= ESelectInfo::Direct*/)
{
	// Clear previous details
	FactionInfoContainer->ClearChildren();

	CurrentSelection = InNewSelection;
	if (CurrentSelection.IsValid())
	{
		TSharedPtr<IPropertyHandle> Property = CurrentSelection.GetValueProperty();

		if (Property.IsValid())
		{
			uint32 Num = 0;
			Property->GetNumChildren(Num);

			/** Populate properties */
			for (uint32 I = 0; I < Num; ++I)
			{
				TSharedPtr<IPropertyHandle> ChildProperty = Property->GetChildHandle(I);

				if (ChildProperty->GetProperty()->GetFName() != GET_MEMBER_NAME_CHECKED(FFactionInfo, Color))
				{
					TSharedRef<SHorizontalBox> PropertyBox = SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.FillWidth(0.35f)
						[
							ChildProperty->CreatePropertyNameWidget()
						]
					+ SHorizontalBox::Slot()
						.HAlign(HAlign_Left)
						.FillWidth(0.65f)
						[
							SNew(SBox)
							.MinDesiredWidth(150.f)
						.MaxDesiredWidth(250.f)
						[
							ChildProperty->CreatePropertyValueWidget()
						]
						];

					FactionInfoContainer->AddSlot()
						.VAlign(VAlign_Top)
						.AutoHeight()
						[
							PropertyBox
						];
				}
			}
		}

		if (ListView.IsValid() && InSelectInfo == ESelectInfo::Direct)
		{
			ListView->SetSelection(InNewSelection, ESelectInfo::OnMouseClick);
		}
	}
}

void FFactionTableCustomization::RefreshView()
{
	AvailableFactions.Empty();
	VisibleFactions.Empty();

	if (!ItemsHandle.IsValid() || !ItemsHandle->IsValidHandle())
		return;

	FName LastSelection = CurrentSelection.IsValid() ? CurrentSelection.Pin()->Name : FName{};


	uint32 Num;
	FPropertyAccess::Result Result = ItemsHandleMap->GetNumElements(Num);
	if (Result != FPropertyAccess::Success)
		return;

	for (uint32 I = 0; I < Num; ++I)
	{
		TSharedRef<IPropertyHandle> ItemProperty = ItemsHandle->GetChildHandle(I).ToSharedRef();

		ItemProperty->GetKeyHandle();

		FName FactionName;
		ItemProperty->GetKeyHandle()->GetValue(FactionName);

		FFactionViewItem Item{ I, ItemProperty };

		AvailableFactions.Add(Item);

		// Apply search Filter
		if (Filter.IsEmpty() ||
			Item.GetName().ToString().Contains(Filter) ||
			Item.GetDisplayName().Contains(Filter))
		{
			VisibleFactions.Add(Item);

			// Keep selection if possible
			if (!LastSelection.IsNone() && Item.GetName() == LastSelection) {
				SetSelection(Item);
			}
		}
	}

	if (ListView.IsValid())
	{
		ListView->RequestListRefresh();
	}
}

void FFactionTableCustomization::OnFilterChanged(const FText& Text)
{
	Filter = Text.ToString();
	RefreshView();
}

FReply FFactionTableCustomization::OnNewFaction()
{
	const FScopedTransaction Transaction(LOCTEXT("Relation_New", "Added new relation"));
	GetOuter()->Modify();

	ItemsHandleMap->AddItem();
	RefreshView();

	return FReply::Handled();
}

FReply FFactionTableCustomization::OnDeleteFaction(const TSharedPtr<FFactionViewItem> Faction)
{
	const FScopedTransaction Transaction(LOCTEXT("Relation_DeleteFaction", "Deleted faction"));
	GetOuter()->Modify();

	// #TODO: ItemsHandleMap->DeleteItem(Index);
	RefreshView();

	return FReply::Handled();
}

FReply FFactionTableCustomization::OnClearFactions()
{
	const FScopedTransaction Transaction(LOCTEXT("Relation_ClearFactions", "Deleted all factions"));

	ItemsHandleMap->Empty();
	RefreshView();

	return FReply::Handled();
}

UObject* FFactionTableCustomization::GetOuter() const
{
	if (!StructHandle.IsValid())
		return nullptr;

	// Customization -> Factions -> Settings
	TArray<UObject*> Objects;
	StructHandle->GetOuterObjects(Objects);

	return Objects.Num() ? Objects[0] : nullptr;
}

#undef LOCTEXT_NAMESPACE
