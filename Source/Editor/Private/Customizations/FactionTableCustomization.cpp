// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "Customizations/FactionTableCustomization.h"

#include "Customizations/SFaction.h"
#include "FactionTable.h"
#include "FactionsSubsystem.h"
#include "Widgets/SFactionColor.h"

#include <DetailLayoutBuilder.h>
#include <DetailWidgetRow.h>
#include <Editor.h>
#include <IDetailChildrenBuilder.h>
#include <PropertyCustomizationHelpers.h>
#include <ScopedTransaction.h>
#include <Widgets/Input/SButton.h>
#include <Widgets/Input/SSearchBox.h>
#include <Widgets/Layout/SScrollBox.h>
#include <Widgets/Layout/SWidgetSwitcher.h>


#define LOCTEXT_NAMESPACE "FFactionTableCustomization"

const FName FFactionTableCustomization::ColumnId(TEXT("Id"));
const FName FFactionTableCustomization::ColumnSelfAttitude(TEXT("SelfAttitude"));
const FName FFactionTableCustomization::ColumnExternalAttitude(TEXT("ExternalAttitude"));
const FName FFactionTableCustomization::ColumnColor(TEXT("Color"));
const FName FFactionTableCustomization::ColumnDelete(TEXT("Delete"));


class SFactionViewItem : public SMultiColumnTableRow<FFactionListItemPtr>
{
public:
	SLATE_BEGIN_ARGS(SFactionViewItem) {}
	/** The widget that owns the tree. We'll only keep a weak reference to it. */
	SLATE_ARGUMENT(TSharedPtr<FFactionTableCustomization>, Customization)
	SLATE_ARGUMENT(FFactionListItemPtr, Item)
	SLATE_END_ARGS()


	/** Construct function for this widget */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		Customization = InArgs._Customization;
		Item = InArgs._Item;

		SMultiColumnTableRow<FFactionListItemPtr>::Construct(
			FSuperRowType::FArguments().Style(FAppStyle::Get(), "DataTableEditor.CellListViewRow"),
			InOwnerTableView);
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
				.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
				.ForegroundColor(FAppStyle::GetSlateColor("DefaultForeground"))
				.OnClicked(Customization.Pin().Get(), &FFactionTableCustomization::OnDeleteFaction,
					Item)[SNew(STextBlock)
							  .Font(FAppStyle::Get().GetFontStyle("FontAwesome.10"))
							  .Text(FText::FromString(FString(TEXT("\xf057"))) /*fa-times-circle*/)];
		}
		else if (Column == FFactionTableCustomization::ColumnSelfAttitude)
		{
			return SNew(SBox).Padding(
				FMargin{5, 0})[Item->GetSelfAttitudeProperty()->CreatePropertyValueWidget()];
		}
		else if (Column == FFactionTableCustomization::ColumnExternalAttitude)
		{
			return SNew(SBox).Padding(
				FMargin{5, 0})[Item->GetExternalAttitudeProperty()->CreatePropertyValueWidget()];
		}
		else if (Column == FFactionTableCustomization::ColumnColor)
		{
			return SNew(SBox)
			.Padding(FMargin{5, 0})
			[
				SNew(SFactionColor, Item->GetColorProperty())
			];
		}

		return SNew(SBox)
		.Padding(FMargin{5, 0})
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
	FText GetIdAsText() const
	{
		return FText::FromName(Item->GetName());
	}

	FReply OnIdNameDoubleClicked(const FGeometry&, const FPointerEvent&);

	void OnIdNameCommited(const FText& InText, ETextCommit::Type InCommitType) const;


	TWeakPtr<FFactionTableCustomization> Customization;
	FFactionListItemPtr Item;

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
		FSlateApplication::Get().GeneratePathToWidgetUnchecked(
			IdNameTextBox.ToSharedRef(), EditableWidgetPath);

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

void FFactionTableCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;
	ListHandle = StructHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionTable, List));

	// Refresh when Num changes
	OnItemsNumChanged = FSimpleDelegate::CreateRaw(this, &FFactionTableCustomization::RefreshList);
	ListHandle->AsMap()->SetOnNumElementsChanged(OnItemsNumChanged);

	RefreshList();

	auto AddButton = PropertyCustomizationHelpers::MakeAddButton(
		FSimpleDelegate::CreateSP(this, &FFactionTableCustomization::OnNewFaction),
		LOCTEXT("AddFactionToolTip", "Adds Faction"));

	auto ClearButton = PropertyCustomizationHelpers::MakeEmptyButton(
		FSimpleDelegate::CreateSP(this, &FFactionTableCustomization::OnClearFactions),
		LOCTEXT("ClearFactionToolTip", "Removes all factions"));

	HeaderRow.NameContent()
	.HAlign(HAlign_Fill)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		[
			StructHandle->CreatePropertyNameWidget()
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.AutoWidth()
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
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()[
			SNew(STextBlock)
			.Text(this, &FFactionTableCustomization::GetHeaderValueText)
		]
		+ SHorizontalBox::Slot()
		.Padding(2.0f)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			AddButton
		]
		+ SHorizontalBox::Slot()
		.Padding(2.0f)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			ClearButton
		]
	];

	GEditor->RegisterForUndo(this);
}

void FFactionTableCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
	IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// Create descriptor details
	auto& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Automatic;
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.bAllowFavoriteSystem = false;
	DetailsViewArgs.bShowOptions = false;
	DetailsViewArgs.bShowPropertyMatrixButton = false;
	DetailsViewArgs.bShowKeyablePropertiesOption = false;
	DetailsViewArgs.bShowAnimatedPropertiesOption = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	FStructureDetailsViewArgs StructDetailView;
	DescriptorDetailsView = PropertyEditorModule.CreateStructureDetailView(
		DetailsViewArgs, StructDetailView, {}, LOCTEXT("FactionDetailsName", "Descriptor"));
	DescriptorDetailsView->GetOnFinishedChangingPropertiesDelegate().AddSP(
		this, &FFactionTableCustomization::OnFinishedChangingProperties);

	TSharedRef<SScrollBar> VerticalScrollBar =
		SNew(SScrollBar).Orientation(Orient_Vertical).Thickness(FVector2D(8.0f, 8.0f));

	TSharedRef<SHeaderRow> ListHeaderRow =
	SNew(SHeaderRow)
	+ SHeaderRow::Column(ColumnDelete)
	.HAlignCell(HAlign_Fill)
	.VAlignCell(VAlign_Fill)
	.VAlignHeader(VAlign_Center)
	.FixedWidth(22.f)
	[
		SNew(STextBlock)
		.Text(LOCTEXT("FactionColumnDelete", ""))
	]
	+ SHeaderRow::Column(ColumnId)
	.HAlignCell(HAlign_Fill)
	.VAlignCell(VAlign_Center)
	.VAlignHeader(VAlign_Center)
	.FillWidth(1.f)
	[
		SNew(STextBlock)
		.Text(LOCTEXT("FactionColumnId", "Id"))
		.ToolTipText(LOCTEXT("FactionColumnIdTooltip",
			"A faction's Id serves as its unique identifier. For setting a name use "
			"the Display Name"))
	]
	+ SHeaderRow::Column(ColumnSelfAttitude)
	.HAlignCell(HAlign_Center)
	.VAlignCell(VAlign_Center)
	.VAlignHeader(VAlign_Center)
	.FixedWidth(90.f)
	[
		SNew(STextBlock)
		.Text(LOCTEXT("FactionColumnSelfAttitude", "To Self"))
	]
	+ SHeaderRow::Column(ColumnExternalAttitude)
	.HAlignCell(HAlign_Center)
	.VAlignCell(VAlign_Center)
	.VAlignHeader(VAlign_Center)
	.FixedWidth(90.f)
	[
		SNew(STextBlock)
		.Text(LOCTEXT("FactionColumnExternalAttitude", "To Others"))
	]
	+ SHeaderRow::Column(ColumnColor)
	.HAlignCell(HAlign_Center)
	.VAlignCell(VAlign_Center)
	.VAlignHeader(VAlign_Center)
	.FixedWidth(70.f)[
		SNew(STextBlock)
		.Text(LOCTEXT("FactionColumnColor", "Color"))
	];

	ListView = SNew(SListView<FFactionListItemPtr>)
		.ListItemsSource(&VisibleFactions)
		.HeaderRow(ListHeaderRow)
		.OnGenerateRow(this, &FFactionTableCustomization::MakeListRow)
		//.OnListViewScrolled(this, &FFactionTableCustomization::OnScrolled)
		.OnSelectionChanged(this, &FFactionTableCustomization::SetSelection)
		.ExternalScrollbar(VerticalScrollBar)
		.ConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible)
		.SelectionMode(ESelectionMode::Single)
		.AllowOverscroll(EAllowOverscroll::No);

	StructBuilder.AddCustomRow(LOCTEXT("FactionsPropertySearch", "Factions"))
		.NameContent()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Fill)[SNew(SBox).Padding(FMargin{0, 0, 0, 8})
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
		.Padding(FMargin{0, 0, 0, 8})
		[
			DescriptorDetailsView->GetWidget().ToSharedRef()
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

void FFactionTableCustomization::PostUndo(bool bSuccess)
{
	if (bSuccess && StructHandle.IsValid())
	{
		RefreshList();
		UpdateSelectedCopy();
	}
}

void FFactionTableCustomization::PostRedo(bool bSuccess)
{
	if (bSuccess && StructHandle.IsValid())
	{
		RefreshList();
		UpdateSelectedCopy();
	}
}

void FFactionTableCustomization::SetSelection(
	FFactionListItemPtr InNewSelection, ESelectInfo::Type InSelectInfo /*= ESelectInfo::Direct*/)
{
	if (CurrentSelection == InNewSelection)
	{
		return;
	}

	// Clear previous details
	OnSelectedDescriptorChanged = {};

	CurrentSelection = InNewSelection;
	if (CurrentSelection.IsValid())
	{
		SelectedDescriptorCopy = MakeShared<FStructOnScope>(FFactionDescriptor::StaticStruct());
		if (UObject* Outer = GetOuter())
		{
			SelectedDescriptorCopy->SetPackage(Outer->GetOutermost());
		}

		if (UpdateSelectedCopy())
		{
			DescriptorDetailsView->SetStructureData(SelectedDescriptorCopy);
		}

		// Listen for external changes to the descriptor to update the copy
		OnSelectedDescriptorChanged = FSimpleDelegate::CreateLambda([this]() {
			UpdateSelectedCopy();
		});
		CurrentSelection.Pin()->GetValueProperty()->SetOnChildPropertyValueChanged(OnSelectedDescriptorChanged);
	}
	else
	{
		// Nothing set, clear selection
		DescriptorDetailsView->SetStructureData({});
		SelectedDescriptorCopy = {};
	}
}

void FFactionTableCustomization::RefreshList()
{
	if (!ListHandle.IsValid() || !ListHandle->IsValidHandle())
	{
		AvailableFactions.Empty();
		VisibleFactions.Empty();
		return;
	}

	const int32 LastSelectedIndex = CurrentSelection.IsValid() ? CurrentSelection.Pin()->Id : INDEX_NONE;

	uint32 Num;
	FPropertyAccess::Result Result = ListHandle->AsMap()->GetNumElements(Num);
	if (Result != FPropertyAccess::Success)
		return;

	AvailableFactions.SetNum(Num);
	for (uint32 I = 0; I < Num; ++I)
	{
		auto& Faction = AvailableFactions[I];
		if (!Faction)
		{
			Faction = MakeShared<FFactionListItem>();
		}

		TSharedPtr<IPropertyHandle> Property = ListHandle->GetChildHandle(I);

		Faction->Id = I;
		Faction->Property = Property->IsValidHandle() ? Property : TSharedPtr<IPropertyHandle>{};
	}

	// Filter available factions
	VisibleFactions.Empty(Num);
	for (const auto& Item : AvailableFactions)
	{
		const FName ItemName = Item->GetName();
		if (Filter.IsEmpty() || ItemName.ToString().Contains(Filter) ||
			Item->GetDisplayName().Contains(Filter))
		{
			VisibleFactions.Add(Item);
		}
	}

	if (ListView.IsValid())
	{
		ListView->RebuildList();
	}

	// Keep selection if possible
	if (LastSelectedIndex != INDEX_NONE && !VisibleFactions.IsEmpty())
	{
		const int32 Index = FMath::Min(LastSelectedIndex, VisibleFactions.Num() - 1);
		SetSelection(VisibleFactions[Index]);
	}
}

TSharedRef<ITableRow> FFactionTableCustomization::MakeListRow(
	FFactionListItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SFactionViewItem, OwnerTable)
		.Customization(SharedThis(this)).Item(Item);
}

void FFactionTableCustomization::OnFilterChanged(const FText& Text)
{
	Filter = Text.ToString();
	RefreshList();
}

void FFactionTableCustomization::OnNewFaction()
{
	const FScopedTransaction Transaction(LOCTEXT("Relation_New", "Add relation"));
	GetOuter()->Modify();

	ListHandle->AsMap()->AddItem();
}

FReply FFactionTableCustomization::OnDeleteFaction(FFactionListItemPtr Item)
{
	const FScopedTransaction Transaction(LOCTEXT("Relation_DeleteFaction", "Delete faction"));
	GetOuter()->Modify();

	ListHandle->AsMap()->DeleteItem(Item->Id);
	return FReply::Handled();
}

void FFactionTableCustomization::OnFactionIdChange(
	const FText& NewIdText, ETextCommit::Type CommitInfo, const FFactionListItemPtr& Item)
{
	check(Item.IsValid());

	const FScopedTransaction Transaction(LOCTEXT("Faction_Rename", "Rename faction"));
	GetOuter()->Modify();

	FName NewId{*NewIdText.ToString()};
	Item->GetKeyProperty()->SetValue(NewId);

	RefreshList();
}

void FFactionTableCustomization::OnClearFactions()
{
	const FScopedTransaction Transaction(LOCTEXT("Faction_ClearFactions", "Delete all factions"));
	GetOuter()->Modify();

	// For some reason Empty is not deleting children, therefore, we will delete one by one
	ListHandle->AsMap()->Empty();
}

void FFactionTableCustomization::OnFinishedChangingProperties(
	const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (CurrentSelection.IsValid())
	{
		const FScopedTransaction Transaction(LOCTEXT("Faction_EditDescriptor", "Edit Descriptor"));
		GetOuter()->Modify();

		TSharedPtr<IPropertyHandle> Property = CurrentSelection.Pin()->GetValueProperty();
		void* Data = nullptr;
		if (Property.IsValid() && Property->GetValueData(Data) == FPropertyAccess::Result::Success)
		{
			Property->NotifyPreChange();
			UScriptStruct* Type = FFactionDescriptor::StaticStruct();
			Type->GetCppStructOps()->Copy((uint8*) Data, SelectedDescriptorCopy->GetStructMemory(), 1);
			Property->NotifyPostChange(PropertyChangedEvent.ChangeType);
		}
	}
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
	if (ListHandle->AsMap()->GetNumElements(Num) != FPropertyAccess::Success)
		return FText::GetEmpty();

	return FText::Format(LOCTEXT("ValueDescription", "{0} factions"), FText::AsNumber(Num));
}

bool FFactionTableCustomization::UpdateSelectedCopy()
{
	if (SelectedDescriptorCopy)
	{
		TSharedPtr<IPropertyHandle> Property = CurrentSelection.Pin()->GetValueProperty();
		void* Data = nullptr;
		if (Property.IsValid() && Property->GetValueData(Data) == FPropertyAccess::Result::Success)
		{
			UScriptStruct* Type = FFactionDescriptor::StaticStruct();
			return Type->GetCppStructOps()->Copy(SelectedDescriptorCopy->GetStructMemory(), (uint8*) Data, 1);
		}
	}
	return false;
}

#undef LOCTEXT_NAMESPACE
