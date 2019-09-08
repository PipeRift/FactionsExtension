// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "Customizations/FactionTableCustomization.h"

#include <Editor.h>
#include <ScopedTransaction.h>
#include <DetailWidgetRow.h>
#include <IDetailChildrenBuilder.h>
#include <Widgets/Layout/SScrollBox.h>
#include <Widgets/Input/SSearchBox.h>
#include <Widgets/Input/SButton.h>
#include <Widgets/Layout/SWidgetSwitcher.h>

#include "Customizations/SFaction.h"

#include "FactionsSettings.h"
#include "FactionTable.h"
#include "Widgets/SFactionColor.h"


#define LOCTEXT_NAMESPACE "FFactionTableCustomization"

const FName FFactionTableCustomization::ColumnId("Id");
const FName FFactionTableCustomization::ColumnColor("Color");
const FName FFactionTableCustomization::ColumnDelete("Delete");


class SFactionViewItem : public SMultiColumnTableRow<FFactionViewItemPtr>
{
public:

	SLATE_BEGIN_ARGS(SFactionViewItem) {}
		/** The widget that owns the tree. We'll only keep a weak reference to it. */
		SLATE_ARGUMENT(TSharedPtr<FFactionTableCustomization>, Customization)
		SLATE_ARGUMENT(FFactionViewItemPtr, Item)
	SLATE_END_ARGS()


	/** Construct function for this widget */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		Customization = InArgs._Customization;
		Item = InArgs._Item;

		SMultiColumnTableRow<FFactionViewItemPtr>::Construct(
			FSuperRowType::FArguments()
			.Style(FEditorStyle::Get(), "DataTableEditor.CellListViewRow"),
			InOwnerTableView
		);
	}

	/** Overridden from SMultiColumnTableRow.  Generates a widget for this column of the list view. */
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& Column) override
	{
		if (Column == FFactionTableCustomization::ColumnDelete)
		{
			return SNew(SButton)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			.ContentPadding(FMargin{0, 1, 0, 0})
			.ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
			.ForegroundColor(FEditorStyle::GetSlateColor("DefaultForeground"))
			.OnClicked(Customization.Pin().Get(), &FFactionTableCustomization::OnDeleteFaction, Item)
			[
				SNew(STextBlock)
				.Font(FEditorStyle::Get().GetFontStyle("FontAwesome.10"))
				.Text(FText::FromString(FString(TEXT("\xf057"))) /*fa-times-circle*/)
			];
		}
		else if (Column == FFactionTableCustomization::ColumnColor)
		{
			return SNew(SBox)
			.Padding(FMargin{ 5, 3 })
			[
				SNew(SFactionColor, Item->GetColorProperty())
			];
		}

		return SNew(SBox)
		.Padding(FMargin{ 5, 0 })
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
					.Text(this, &SFactionViewItem::GetIdAsText)
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
					.Text(this, &SFactionViewItem::GetIdAsText)
					.ToolTipText(LOCTEXT("EditNameId_Tooltip", "Replace a faction's Id"))
					.RevertTextOnEscape(true)
					.ClearKeyboardFocusOnCommit(true)
					.OnTextCommitted(this, &SFactionViewItem::OnIdNameCommited)
				]
			]
		];
	}

private:

	FText GetIdAsText() const { return FText::FromName(Item->GetName()); }

	FReply OnIdNameDoubleClicked(const FGeometry&, const FPointerEvent&);

	void OnIdNameCommited(const FText& InText, ETextCommit::Type InCommitType) const;


	TWeakPtr<FFactionTableCustomization> Customization;
	FFactionViewItemPtr Item;

	TSharedPtr<SWidgetSwitcher> IdNameSwitcher;
	TSharedPtr<SEditableTextBox> IdNameTextBox;
};


FReply SFactionViewItem::OnIdNameDoubleClicked(const FGeometry&, const FPointerEvent&)
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
	if (InCommitType == ETextCommit::OnEnter || InCommitType == ETextCommit::OnUserMovedFocus)
	{
		if (Customization.IsValid())
		{
			Customization.Pin()->OnFactionIdChange(InText, InCommitType, Item);
		}

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

	if (!ItemsHandle.IsValid() || !ItemsHandle->IsValidHandle())
		return;

	ItemsHandleMap = ItemsHandle->AsMap();

	// Refresh when Num changes
	OnItemsNumChanged = FSimpleDelegate::CreateRaw(this, &FFactionTableCustomization::RefreshView);
	ItemsHandleMap->SetOnNumElementsChanged(OnItemsNumChanged);

	RefreshView();

	HeaderRow
	.NameContent()
	.VAlign(VAlign_Top)
	[
		SNew(SBox)
		.Padding(FMargin{ 0, 10 })
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
	]
	.ValueContent()
	.HAlign(HAlign_Left)
	.VAlign(VAlign_Center)
	[
		SNew(SBox)
		.Padding(FMargin{ 10, 0 })
		[
			SNew(STextBlock)
			.Text(this, &FFactionTableCustomization::GetHeaderValueText)
		]
	];

	GEditor->RegisterForUndo(this);
}

void FFactionTableCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedRef<SScrollBar> VerticalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Vertical)
		.Thickness(FVector2D(8.0f, 8.0f));

	TSharedRef<SHeaderRow> ListHeaderRow = SNew(SHeaderRow)
	+ SHeaderRow::Column(ColumnDelete)
	.HAlignCell(HAlign_Fill)
	.VAlignCell(VAlign_Fill)
	.VAlignHeader(VAlign_Center)
	.FixedWidth(22.f)
	[
		SNew(STextBlock).Text(LOCTEXT("FactionColumnDelete", ""))
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
		.Padding(5, 0)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("FactionColumnId", "Faction Id"))
			.ToolTipText(LOCTEXT("FactionColumnIdTooltip", "A faction's Id serves as its unique identifier. For setting a name use the Display Name"))
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.Padding(5, 0)
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
	.HAlignCell(HAlign_Center)
	.VAlignCell(VAlign_Center)
	.VAlignHeader(VAlign_Center)
	.FixedWidth(70.f)
	.HeaderContentPadding(FMargin(5, 3))
	[
		SNew(STextBlock).Text(LOCTEXT("FactionColumnColor", "Color"))
	];

	ListView = SNew(SListView<FFactionViewItemPtr>)
		.ListItemsSource(&VisibleFactions)
		.HeaderRow(ListHeaderRow)
		.OnGenerateRow(this, &FFactionTableCustomization::MakeFactionWidget)
		//.OnListViewScrolled(this, &FFactionTableCustomization::OnScrolled)
		.OnSelectionChanged(this, &FFactionTableCustomization::SetSelection)
		.ExternalScrollbar(VerticalScrollBar)
		.ConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible)
		.SelectionMode(ESelectionMode::Single)
		.AllowOverscroll(EAllowOverscroll::No);

	StructBuilder.AddCustomRow(LOCTEXT("FactionsPropertySearch", "Factions"))
	.NameContent()
	.VAlign(VAlign_Top)
	[
		SNew(SBox)
		.Padding(FMargin{ 0,10,0,20 })
		.VAlign(VAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			[
				SNew(SBox)
				.MaxDesiredHeight(180.f)
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
		.Padding(10)
		.MinDesiredHeight(180.f)
		[
			SAssignNew(FactionInfoContainer, SVerticalBox)
		]
	];

	if (!CurrentSelection.IsValid() && AvailableFactions.Num() > 0)
	{
		SetSelection(AvailableFactions[0]);
	}
}

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

TSharedRef<ITableRow> FFactionTableCustomization:: MakeFactionWidget(FFactionViewItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable)
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

void FFactionTableCustomization::SetSelection(FFactionViewItemPtr InNewSelection, ESelectInfo::Type InSelectInfo /*= ESelectInfo::Direct*/)
{
	// Clear previous details
	FactionInfoContainer->ClearChildren();

	CurrentSelection = InNewSelection;
	if (CurrentSelection.IsValid())
	{
		TSharedPtr<IPropertyHandle> Property = CurrentSelection.Pin()->GetValueProperty();
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
	if (!ItemsHandle.IsValid() || !ItemsHandle->IsValidHandle())
	{
		AvailableFactions.Empty();
		VisibleFactions.Empty();
		return;
	}

	int32 LastSelection = CurrentSelection.IsValid() ? CurrentSelection.Pin()->Id : INDEX_NONE;

	uint32 Num;
	FPropertyAccess::Result Result = ItemsHandleMap->GetNumElements(Num);
	if (Result != FPropertyAccess::Success)
		return;

	AvailableFactions.Empty(Num);
	VisibleFactions.Empty(Num);
	for (uint32 I = 0; I < Num; ++I)
	{
		TSharedRef<IPropertyHandle> ItemProperty = ItemsHandle->GetChildHandle(I).ToSharedRef();
		if (ItemProperty->IsValidHandle())
		{
			ItemProperty->GetKeyHandle();

			FName FactionName;
			ItemProperty->GetKeyHandle()->GetValue(FactionName);

			FFactionViewItemPtr Item = MakeShared<FFactionViewItem>(I, ItemProperty);
			AvailableFactions.Add(Item);

			const FName ItemName = Item->GetName();

			// Apply search Filter
			if (Filter.IsEmpty() ||
				ItemName.ToString().Contains(Filter) ||
				Item->GetDisplayName().Contains(Filter))
			{
				VisibleFactions.Add(Item);

				// Keep selection if possible
				if (LastSelection != INDEX_NONE && Item->Id == LastSelection) {
					SetSelection(Item);
				}
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

	return FReply::Handled();
}

FReply FFactionTableCustomization::OnDeleteFaction(FFactionViewItemPtr Item)
{
	const FScopedTransaction Transaction(LOCTEXT("Relation_DeleteFaction", "Deleted faction"));
	GetOuter()->Modify();

	ItemsHandleMap->DeleteItem(Item->Id);

	return FReply::Handled();
}

void FFactionTableCustomization::OnFactionIdChange(const FText& NewIdText, ETextCommit::Type CommitInfo, const FFactionViewItemPtr& Item)
{
	check(Item.IsValid());

	const FScopedTransaction Transaction(LOCTEXT("Faction_Rename", "Rename faction"));
	GetOuter()->Modify();

	FName NewId{ *NewIdText.ToString() };
	Item->GetKeyProperty()->SetValue(NewId);

	RefreshView();
}

FReply FFactionTableCustomization::OnClearFactions()
{
	const FScopedTransaction Transaction(LOCTEXT("Relation_ClearFactions", "Deleted all factions"));
	GetOuter()->Modify();

	//For some reason Empty is not deleting children, therefore, we will delete one by one
	ItemsHandleMap->Empty();

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

FText FFactionTableCustomization::GetHeaderValueText() const
{
	uint32 Num;
	if (ItemsHandleMap->GetNumElements(Num) != FPropertyAccess::Success)
		return FText::GetEmpty();

	return FText::Format(LOCTEXT("ValueDescription", "{0} factions"), FText::AsNumber(Num));
}

#undef LOCTEXT_NAMESPACE
