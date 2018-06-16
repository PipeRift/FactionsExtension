// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "FactionsSettingsDetails.h"

#include "Settings/EditorExperimentalSettings.h"
#include "DetailLayoutBuilder.h"
#include "ScopedTransaction.h"
#include "IDetailPropertyRow.h"
#include "DetailCategoryBuilder.h"
#include "Widgets/Input/SButton.h"
#include "Engine/CurveTable.h"
#include "DetailWidgetRow.h"
#include "Widgets/Text/STextBlock.h"
#include "SGridPanel.h"
#include "SInlineEditableTextBlock.h"
#include "SSearchBox.h"

#include "Database/FactionsSettings.h"


#define LOCTEXT_NAMESPACE "FFactionsSettingsDetails"

const FName FFactionsSettingsDetails::ColumnId("Id");
const FName FFactionsSettingsDetails::ColumnColor("Color");
const FName FFactionsSettingsDetails::ColumnDelete("Delete");


class SFactionViewItem : public SMultiColumnTableRow<TSharedPtr<FFactionInfoMapItem>>
{
public:
	SLATE_BEGIN_ARGS(SFactionViewItem) {}
	/** The widget that owns the tree.  We'll only keep a weak reference to it. */
	SLATE_ARGUMENT(TSharedPtr<FFactionsSettingsDetails>, Details)
		SLATE_ARGUMENT(TSharedPtr<FFactionInfoMapItem>, Faction)
		SLATE_END_ARGS()

		/** Construct function for this widget */
		void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		Details = InArgs._Details;
		Faction = InArgs._Faction;

		if (Faction.IsValid())
		{
			ColorProperty = Faction->Property->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionInfo, Color));
		}

		SMultiColumnTableRow<TSharedPtr<FFactionInfoMapItem>>::Construct(
			FSuperRowType::FArguments()
			.Style(FEditorStyle::Get(), "DataTableEditor.CellListViewRow"),
			InOwnerTableView
		);
	}

	/** Overridden from SMultiColumnTableRow.  Generates a widget for this column of the list view. */
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& Column) override
	{
		TSharedPtr<FFactionsSettingsDetails> CustomizationPtr = Details.Pin();

		if (Column == FFactionsSettingsDetails::ColumnColor)
		{
			return SNew(STextBlock)
				.Text(LOCTEXT("ValueColor", "Color"));
		}
		else if (Column == FFactionsSettingsDetails::ColumnDelete)
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
				.OnClicked(Details.Pin().Get(), &FFactionsSettingsDetails::OnDeleteFaction, Faction)
			];
		}

		return SNew(SBox)
		.Padding(5.0f)
		.MinDesiredHeight(20)
		[
			SAssignNew(NameTextBlock, SInlineEditableTextBlock)
			.Text(this, &SFactionViewItem::GetEditableNameAsText)
			//.OnVerifyTextChanged(this, &SDialogueGraphNode_Area::OnVerifyNameTextChanged)
			//.OnTextCommitted(this, &SDialogueGraphNode_Area::OnNameTextCommited)
			.ModiferKeyForNewLine(EModifierKey::Shift)
		];
	}

private:

	FText GetEditableNameAsText() const {
		return FText::FromName(Faction->Name);
	}

	/** Weak reference to the database customization that owns our list */
	TWeakPtr<FFactionsSettingsDetails> Details;

	TSharedPtr<FFactionInfoMapItem> Faction;

	TSharedPtr<IPropertyHandle> ColorProperty;

	TSharedPtr<SInlineEditableTextBlock> NameTextBlock;
};


TSharedRef<IDetailCustomization> FFactionsSettingsDetails::MakeInstance()
{
	return MakeShareable(new FFactionsSettingsDetails);
}

void FFactionsSettingsDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	CustomizeFactionsDetails(DetailBuilder);
}

void FFactionsSettingsDetails::CustomizeFactionsDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);

	if (Objects.Num() && Objects[0] != nullptr)
	{
		FactionsMapHandle = DetailBuilder.GetProperty(UFactionsSettings::GetFactionsPropertyName());
		if(FactionsMapHandle->IsValidHandle())
		{
			Settings = CastChecked<UFactionsSettings>(Objects[0].Get());
			RefreshFactions();


			IDetailCategoryBuilder& Category = DetailBuilder.EditCategory(TEXT("Factions"));

			IDetailPropertyRow& FactionsPropertyRow = Category.AddProperty(FactionsMapHandle);
			TSharedPtr<SWidget> DefaultNameWidget;
			TSharedPtr<SWidget> DefaultValueWidget;
			FactionsPropertyRow.GetDefaultWidgets(DefaultNameWidget, DefaultValueWidget);
			FactionsPropertyRow.CustomWidget();

			{
				TSharedRef<SHeaderRow> HeaderRow = SNew(SHeaderRow)
				+ SHeaderRow::Column(ColumnId)
				.HAlignCell(HAlign_Fill)
				.FillWidth(0.8f)
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
							.OnTextChanged(this, &FFactionsSettingsDetails::OnFactionFilterChanged)
						]
					]
				]
				+ SHeaderRow::Column(ColumnColor)
				.HAlignCell(HAlign_Left)
				.FillWidth(0.2f)
				.HeaderContentPadding(FMargin(0, 3))
				[
					SNew(STextBlock).Text(LOCTEXT("FactionColumnColor", "Color"))
				]
				+ SHeaderRow::Column(ColumnDelete)
				.HAlignCell(HAlign_Right)
				.FixedWidth(20.f)
				.HeaderContentPadding(FMargin(0, 3))
				[
					SNew(STextBlock).Text(LOCTEXT("FactionColumnDelete", ""))
				];

				FactionsListView = SNew(SListView<TSharedPtr<FFactionInfoMapItem>>)
					.ListItemsSource(&VisibleFactions)
					.HeaderRow(HeaderRow)
					.OnGenerateRow(this, &FFactionsSettingsDetails::MakeFactionWidget)
					//.OnListViewScrolled(this, &FFactionsSettingsDetails::OnFactionsScrolled)
					.OnSelectionChanged(this, &FFactionsSettingsDetails::SetFactionSelection)
					.ConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible)
					.SelectionMode(ESelectionMode::Single)
					.AllowOverscroll(EAllowOverscroll::No);
			}

			FDetailWidgetRow& FactionRow = Category.AddCustomRow(LOCTEXT("FactionPropertyName", "Factions"))
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
						.OnClicked(this, &FFactionsSettingsDetails::OnNewFaction)
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
						.OnClicked(this, &FFactionsSettingsDetails::OnClearFactions)
					]
				]
				+ SVerticalBox::Slot()
				[
					FactionsListView.ToSharedRef()
				]
			]
			.ValueContent()
			.HAlign(HAlign_Fill)
			[
				SNew(SBox)
				.Padding(FMargin{ 5, 30, 5, 10 })
				.HeightOverride(350.f)
				[
					SAssignNew(FactionContainer, SVerticalBox)
				]
			];


		}
	}
}

TSharedRef<ITableRow> FFactionsSettingsDetails::MakeFactionWidget(TSharedPtr<FFactionInfoMapItem> Faction, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SFactionViewItem, OwnerTable)
		.Details(SharedThis(this))
		.Faction(Faction);
}

void FFactionsSettingsDetails::SetFactionSelection(const TSharedPtr<FFactionInfoMapItem> InNewSelection, ESelectInfo::Type InSelectInfo)
{
	// Clear previous details
	FactionContainer->ClearChildren();

	CurrentSelection = InNewSelection;
	if (CurrentSelection.IsValid())
	{
		IPropertyHandle& Property = *CurrentSelection->Property.Get();

		uint32 Num = 0;
		Property.GetNumChildren(Num);


		/** Populate properties */
		bool bSkippedColor = false;
		for (uint32 I = 0; I < Num; ++I)
		{
			TSharedPtr<IPropertyHandle> ChildProperty = Property.GetChildHandle(I);

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


				FactionContainer->AddSlot()
				.VAlign(VAlign_Top)
				.AutoHeight()
				[
					PropertyBox
				];
			}
		}
	}
}

void FFactionsSettingsDetails::RefreshFactions()
{
	if (Settings.IsValid() &&
	   (FactionsMapHandle.IsValid() && FactionsMapHandle->IsValidHandle()))
	{
		TMap<FName, FFactionInfo>& Factions = Settings->Internal_GetFactions();
		// Reserve space while cleaning
		AvailableFactions.Empty(Factions.Num());
		VisibleFactions.Empty(Factions.Num());

		uint32 Num;
		FactionsMapHandle->GetNumChildren(Num);

		TArray<FName> Keys;
		Factions.GetKeys(Keys);
		for (uint32 I = 0; I < Num; ++I)
		{
			if ((int32)I < Keys.Num())
			{
				TSharedPtr<IPropertyHandle> Property = FactionsMapHandle->GetChildHandle(I);
				check(Property.IsValid());

				TSharedPtr<FFactionInfoMapItem> Faction{ MakeShared<FFactionInfoMapItem>(I, Keys[I], Property) };
				AvailableFactions.Add(Faction);


				FString DisplayName {};

				const FFactionInfo* FoundInfo = Settings->Factions.Find(Faction->Name);
				if (FoundInfo)
				{
					DisplayName = FoundInfo->DisplayName.ToString();
				}

				// Apply Filter
				if (SearchFilter.IsEmpty() ||
					Faction->Name.ToString().Contains(SearchFilter) ||
					DisplayName.Contains(SearchFilter))
				{
					VisibleFactions.Add(Faction);
				}
			}
		}
	}
	else
	{
		AvailableFactions.Empty();
		VisibleFactions.Empty();
	}

	if (FactionsListView.IsValid())
	{
		FactionsListView->RequestListRefresh();
	}
}

void FFactionsSettingsDetails::OnFactionFilterChanged(const FText& Text)
{
	SearchFilter = Text.ToString();
	RefreshFactions();
}

FReply FFactionsSettingsDetails::OnNewFaction()
{
	if (Settings.IsValid())
	{
		const FScopedTransaction Transaction(LOCTEXT("Faction_New", "Add new faction"));
		Settings->Modify();

		FactionsMapHandle->AsMap()->AddItem();
		RefreshFactions();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply FFactionsSettingsDetails::OnClearFactions()
{
	if (Settings.IsValid())
	{
		const FScopedTransaction Transaction(LOCTEXT("Faction_Clear", "Delete all factions"));
		Settings->Modify();

		FactionsMapHandle->AsMap()->Empty();
		RefreshFactions();

		//Clear selection
		SetFactionSelection(nullptr);

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply FFactionsSettingsDetails::OnDeleteFaction(const TSharedPtr<FFactionInfoMapItem> Faction)
{
	if (Settings.IsValid())
	{
		const FScopedTransaction Transaction(LOCTEXT("Faction_Delete", "Delete faction"));
		Settings->Modify();

		FactionsMapHandle->AsMap()->DeleteItem(Faction->Id);
		RefreshFactions();

		// Clean selection if removed
		if (Faction == CurrentSelection)
		{
			SetFactionSelection(nullptr);
		}

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE
