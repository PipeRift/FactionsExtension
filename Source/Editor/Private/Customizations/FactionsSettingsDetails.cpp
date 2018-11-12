// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "FactionsSettingsDetails.h"

#include "Editor.h"
#include <Framework/Application/SlateApplication.h>
#include <Layout/WidgetPath.h>
#include <Settings/EditorExperimentalSettings.h>
#include <DetailLayoutBuilder.h>
#include <ScopedTransaction.h>
#include <IDetailPropertyRow.h>
#include <DetailCategoryBuilder.h>
#include <Engine/CurveTable.h>
#include <DetailWidgetRow.h>

#include <Widgets/Input/SButton.h>
#include <Widgets/Text/STextBlock.h>
#include <Widgets/Colors/SColorBlock.h>
#include <Widgets/Colors/SColorPicker.h>
#include <Widgets/Layout/SGridPanel.h>
#include <Widgets/Layout/SWidgetSwitcher.h>
#include <Widgets/Input/SEditableTextBox.h>
#include <Widgets/Input/SSearchBox.h>

#include "Database/FactionsSettings.h"


#define LOCTEXT_NAMESPACE "FFactionsSettingsDetails"

const FName FFactionsSettingsDetails::ColumnSelect("Select");
const FName FFactionsSettingsDetails::ColumnId("Id");
const FName FFactionsSettingsDetails::ColumnColor("Color");
const FName FFactionsSettingsDetails::ColumnDelete("Delete");


/************************************************************************
* SFactionViewItem
*/

void SFactionViewItem::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	Details = InArgs._Details;
	Faction = InArgs._Faction;

	if (Faction.IsValid())
	{
		auto Property = Faction->GetProperty();
		if (Property.IsValid())
		{
			InfoColorProperty = Property->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionInfo, Color));

			bColorIsLinear = CastChecked<UStructProperty>(InfoColorProperty->GetProperty())->Struct->GetFName() == NAME_LinearColor;
			bColorIgnoreAlpha = InfoColorProperty->GetProperty()->HasMetaData(TEXT("HideAlphaChannel"));
		}
	}
	bDontUpdateWhileEditingColor = true;

	SMultiColumnTableRow<TSharedPtr<FFactionInfoMapItem>>::Construct(
		FSuperRowType::FArguments()
		.Style(FEditorStyle::Get(), "DataTableEditor.CellListViewRow"),
		InOwnerTableView
	);
}

TSharedRef<SWidget> SFactionViewItem::GenerateWidgetForColumn(const FName& Column)
{
	TSharedPtr<FFactionsSettingsDetails> CustomizationPtr = Details.Pin();

	if (Column == FFactionsSettingsDetails::ColumnSelect)
	{
		return SNew(STextBlock).Text(LOCTEXT("FactionColumnSelect_Value", "⊙"));
	}
	else if (Column == FFactionsSettingsDetails::ColumnColor)
	{
		return CreateColorWidget(InfoColorProperty);
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

FReply SFactionViewItem::OnIdNameDoubleClicked()
{
	FReply Reply = FReply::Unhandled();

	if (Details.IsValid())
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
	if (Details.IsValid())
	{
		Details.Pin()->OnFactionIdChange(InText, InCommitType, Faction);
	}

	if (InCommitType == ETextCommit::OnEnter || InCommitType == ETextCommit::OnUserMovedFocus)
	{
		IdNameSwitcher->SetActiveWidgetIndex(0);
	}
}

TSharedRef<SWidget> SFactionViewItem::CreateColorWidget(TWeakPtr<IPropertyHandle> ColorProperty)
{
	FSlateFontInfo NormalText = IDetailLayoutBuilder::GetDetailFont();

	return SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.Padding(0.0f, 2.0f)
	[
		// Displays the color with alpha unless it is ignored
		SAssignNew(ColorPickerParentWidget, SColorBlock)
		.Color(this, &SFactionViewItem::OnGetColorForColorBlock)
		.ShowBackgroundForAlpha(true)
		.IgnoreAlpha(bColorIgnoreAlpha)
		.OnMouseButtonDown(this, &SFactionViewItem::OnMouseButtonDownColorBlock)
		.Size(FVector2D(35.0f, 12.0f))
		.IsEnabled(this, &SFactionViewItem::IsValueEnabled, ColorProperty)
	]
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.Padding(0.0f, 2.0f)
	[
		// Displays the color without alpha
		SNew(SColorBlock)
		.Color(this, &SFactionViewItem::OnGetColorForColorBlock)
		.ShowBackgroundForAlpha(false)
		.IgnoreAlpha(true)
		.OnMouseButtonDown(this, &SFactionViewItem::OnMouseButtonDownColorBlock)
		.Size(FVector2D(35.0f, 12.0f))
	];
}

void SFactionViewItem::CreateColorPicker(bool bUseAlpha)
{
	int32 NumObjects = InfoColorProperty->GetNumOuterObjects();

	SavedPreColorPickerColors.Empty();
	TArray<FString> PerObjectValues;
	InfoColorProperty->GetPerObjectValues(PerObjectValues);

	for (int32 ObjectIndex = 0; ObjectIndex < NumObjects; ++ObjectIndex)
	{
		if (bColorIsLinear)
		{
			FLinearColor Color;
			Color.InitFromString(PerObjectValues[ObjectIndex]);
			SavedPreColorPickerColors.Add(FLinearOrSrgbColor(Color));
		}
		else
		{
			FColor Color;
			Color.InitFromString(PerObjectValues[ObjectIndex]);
			SavedPreColorPickerColors.Add(FLinearOrSrgbColor(Color));
		}
	}

	FLinearColor InitialColor;
	GetColorAsLinear(InitialColor);

	const bool bRefreshOnlyOnOk = bDontUpdateWhileEditingColor || InfoColorProperty->HasMetaData("DontUpdateWhileEditing");

	FColorPickerArgs PickerArgs;
	{
		PickerArgs.bUseAlpha = !bColorIgnoreAlpha;
		PickerArgs.bOnlyRefreshOnMouseUp = false;
		PickerArgs.bOnlyRefreshOnOk = bRefreshOnlyOnOk;
		PickerArgs.sRGBOverride = false;
		PickerArgs.DisplayGamma = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateUObject(GEngine, &UEngine::GetDisplayGamma));
		PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &SFactionViewItem::OnSetColorFromColorPicker);
		PickerArgs.OnColorPickerCancelled = FOnColorPickerCancelled::CreateSP(this, &SFactionViewItem::OnColorPickerCancelled);
		PickerArgs.OnInteractivePickBegin = FSimpleDelegate::CreateSP(this, &SFactionViewItem::OnColorPickerInteractiveBegin);
		PickerArgs.OnInteractivePickEnd = FSimpleDelegate::CreateSP(this, &SFactionViewItem::OnColorPickerInteractiveEnd);
		PickerArgs.InitialColorOverride = InitialColor;
		PickerArgs.ParentWidget = ColorPickerParentWidget;
		PickerArgs.OptionalOwningDetailsView = ColorPickerParentWidget;
		FWidgetPath ParentWidgetPath;
		if (FSlateApplication::Get().FindPathToWidget(ColorPickerParentWidget.ToSharedRef(), ParentWidgetPath))
		{
			PickerArgs.bOpenAsMenu = FSlateApplication::Get().FindMenuInWidgetPath(ParentWidgetPath).IsValid();
		}
	}

	OpenColorPicker(PickerArgs);
}

void SFactionViewItem::OnSetColorFromColorPicker(FLinearColor NewColor)
{
	FString ColorString;
	if (bColorIsLinear)
	{
		ColorString = NewColor.ToString();
	}
	else
	{
		const bool bSRGB = true;
		FColor NewFColor = NewColor.ToFColor(bSRGB);
		ColorString = NewFColor.ToString();
	}

	InfoColorProperty->SetValueFromFormattedString(ColorString, bColorIsInteractive ? EPropertyValueSetFlags::InteractiveChange : 0);
	InfoColorProperty->NotifyFinishedChangingProperties();
}

void SFactionViewItem::OnColorPickerCancelled(FLinearColor OriginalColor)
{
	TArray<FString> PerObjectColors;

	for (int32 ColorIndex = 0; ColorIndex < SavedPreColorPickerColors.Num(); ++ColorIndex)
	{
		if (bColorIsLinear)
		{
			PerObjectColors.Add(SavedPreColorPickerColors[ColorIndex].GetLinear().ToString());
		}
		else
		{
			FColor Color = SavedPreColorPickerColors[ColorIndex].GetSrgb();
			PerObjectColors.Add(Color.ToString());
		}
	}

	if (PerObjectColors.Num() > 0)
	{
		InfoColorProperty->SetPerObjectValues(PerObjectColors);
	}
}

void SFactionViewItem::OnColorPickerInteractiveBegin()
{
	bColorIsInteractive = true;
	GEditor->BeginTransaction(FText::Format(LOCTEXT("SetColorProperty", "Edit {0}"), InfoColorProperty->GetPropertyDisplayName()));
}

void SFactionViewItem::OnColorPickerInteractiveEnd()
{
	bColorIsInteractive = false;

	if (!bDontUpdateWhileEditingColor)
	{
		// pushes the last value from the interactive change without the interactive flag
		FString ColorString;
		InfoColorProperty->GetValueAsFormattedString(ColorString);
		InfoColorProperty->SetValueFromFormattedString(ColorString);
	}

	GEditor->EndTransaction();
}

FPropertyAccess::Result SFactionViewItem::GetColorAsLinear(FLinearColor& OutColor) const
{
	// Default to full alpha in case the alpha component is disabled.
	OutColor.A = 1.0f;

	FString StringValue;
	FPropertyAccess::Result Result = InfoColorProperty->GetValueAsFormattedString(StringValue);

	if (Result == FPropertyAccess::Success)
	{
		if (bColorIsLinear)
		{
			OutColor.InitFromString(StringValue);
		}
		else
		{
			FColor SrgbColor;
			SrgbColor.InitFromString(StringValue);
			OutColor = FLinearColor(SrgbColor);
		}
	}
	else if (Result == FPropertyAccess::MultipleValues)
	{
		OutColor = FLinearColor::White;
	}

	return Result;
}

FLinearColor SFactionViewItem::OnGetColorForColorBlock() const
{
	FLinearColor Color;
	GetColorAsLinear(Color);
	return Color;
}

FReply SFactionViewItem::OnMouseButtonDownColorBlock(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return FReply::Unhandled();
	}

	bool CanShowColorPicker = true;
	if (InfoColorProperty.IsValid() && InfoColorProperty->GetProperty() != nullptr)
	{
		CanShowColorPicker = !InfoColorProperty->IsEditConst();
	}
	if (CanShowColorPicker)
	{
		CreateColorPicker(true /*bUseAlpha*/);
	}

	return FReply::Handled();
}

bool SFactionViewItem::IsValueEnabled(TWeakPtr<IPropertyHandle> WeakHandlePtr) const
{
	if (WeakHandlePtr.IsValid())
	{
		return !WeakHandlePtr.Pin()->IsEditConst();
	}
	return false;
}


/************************************************************************
 * FFactionsSettingsDetails
 */

const FName FFactionsSettingsDetails::NewFactionName{ "New Faction" };

TSharedRef<IDetailCustomization> FFactionsSettingsDetails::MakeInstance()
{
	return MakeShareable(new FFactionsSettingsDetails);
}

void FFactionsSettingsDetails::PostUndo(bool bSuccess)
{
	if (bSuccess && Settings.IsValid())
	{
		RefreshFactions();
	}
}

void FFactionsSettingsDetails::PostRedo(bool bSuccess)
{
	if (bSuccess && Settings.IsValid())
	{
		RefreshFactions();
	}
}

FFactionsSettingsDetails::~FFactionsSettingsDetails()
{
	GEditor->UnregisterForUndo(this);
}

void FFactionsSettingsDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	CustomizeFactionsDetails(DetailBuilder);

	GEditor->RegisterForUndo(this);
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


			TSharedRef<SScrollBar> ListScrollBar = SNew(SScrollBar)
				.Orientation(Orient_Vertical)
				.Thickness(FVector2D(8.0f, 8.0f));

			{
				TSharedRef<SHeaderRow> HeaderRow = SNew(SHeaderRow)
				+ SHeaderRow::Column(ColumnSelect)
				.HAlignCell(HAlign_Center)
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
							.OnTextChanged(this, &FFactionsSettingsDetails::OnFactionFilterChanged)
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

				FactionsListView = SNew(SListView<TSharedPtr<FFactionInfoMapItem>>)
					.ListItemsSource(&VisibleFactions)
					.HeaderRow(HeaderRow)
					.OnGenerateRow(this, &FFactionsSettingsDetails::MakeFactionWidget)
					//.OnListViewScrolled(this, &FFactionsSettingsDetails::OnFactionsScrolled)
					.OnSelectionChanged(this, &FFactionsSettingsDetails::SetFactionSelection)
					.ExternalScrollbar(ListScrollBar)
					.ConsumeMouseWheel(EConsumeMouseWheel::Always)
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
				.VAlign(VAlign_Fill)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.f)
					[
						SNew(SBox)
						.HeightOverride(280.f)
						[
							FactionsListView.ToSharedRef()
						]
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SBox)
						.WidthOverride(14.f)
						[
							ListScrollBar
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
	if (InNewSelection.IsValid())
	{
		TSharedPtr<IPropertyHandle> Property = InNewSelection->GetProperty();

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

					FactionContainer->AddSlot()
					.VAlign(VAlign_Top)
					.AutoHeight()
					[
						PropertyBox
					];
				}
			}
		}

		if (FactionsListView.IsValid() && InSelectInfo == ESelectInfo::Direct)
		{
			FactionsListView->SetSelection(InNewSelection, ESelectInfo::OnMouseClick);
		}
	}
}

void FFactionsSettingsDetails::RefreshFactions()
{
	if (Settings.IsValid() &&
	   (FactionsMapHandle.IsValid() && FactionsMapHandle->IsValidHandle()))
	{
		FName LastSelection = CurrentSelection.IsValid() ? CurrentSelection.Pin()->Name : FName{};

		TMap<FName, FFactionInfo>& Factions = Settings->Internal_GetFactions();
		// Reserve space while cleaning
		AvailableFactions.Empty(Factions.Num());
		VisibleFactions.Empty(Factions.Num());

		TArray<FName> Keys;
		Factions.GetKeys(Keys);
		for (int32 I = 0; I < Keys.Num(); ++I)
		{
			TSharedPtr<FFactionInfoMapItem> Faction{ MakeShared<FFactionInfoMapItem>(I, Keys[I], FactionsMapHandle) };
			AvailableFactions.Add(Faction);
		}

		for (const auto& Faction : AvailableFactions)
		{
			FString DisplayName{};
			const FFactionInfo* FoundInfo = Settings->Factions.Find(Faction->Name);
			if (FoundInfo)
				DisplayName = FoundInfo->DisplayName.ToString();

			// Apply search Filter
			if (SearchFilter.IsEmpty() ||
				Faction->Name.ToString().Contains(SearchFilter) ||
				DisplayName.Contains(SearchFilter))
			{
				VisibleFactions.Add(Faction);

				// Keep selection if possible
				if (!LastSelection.IsNone() && Faction->Name == LastSelection) {
					SetFactionSelection(Faction);
				}
			}
		}
	}
	else
	{
		AvailableFactions.Empty();
		VisibleFactions.Empty();

		SetFactionSelection(nullptr);
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
	if (Settings.IsValid() && !Settings->Factions.Contains(NewFactionName))
	{
		const FScopedTransaction Transaction(LOCTEXT("Faction_New", "Add new faction"));
		Settings->Modify();

		Settings->Factions.Emplace(NewFactionName);
		Settings->SaveConfig(CPF_Config);

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

		Settings->Factions.Empty();
		Settings->SaveConfig(CPF_Config);

		RefreshFactions();

		//Clear selection
		SetFactionSelection(nullptr);

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void FFactionsSettingsDetails::OnFactionIdChange(const FText& NewIdText, ETextCommit::Type CommitInfo, const TSharedPtr<FFactionInfoMapItem> Faction)
{
	check(Faction.IsValid());

	FName NewId{ *NewIdText.ToString() };
	if (!Settings->Factions.Contains(NewId))
	{
		const FScopedTransaction Transaction(LOCTEXT("Faction_Rename", "Rename faction"));
		Settings->Modify();

		//Copy faction info
		FFactionInfo Info = *Settings->Factions.Find(Faction->Name);

		Settings->Factions.Remove(Faction->Name);
		Settings->Factions.Add(NewId, MoveTemp(Info));
		Settings->SaveConfig(CPF_Config);

		// Selection will be kept if we update the name and then refresh
		Faction->Name = NewId;
		RefreshFactions();
	}
}

FReply FFactionsSettingsDetails::OnDeleteFaction(const TSharedPtr<FFactionInfoMapItem> Faction)
{
	if (Settings.IsValid())
	{
		const FScopedTransaction Transaction(LOCTEXT("Faction_Delete", "Delete faction"));
		Settings->Modify();

		Settings->Factions.Remove(Faction->Name);
		Settings->SaveConfig(CPF_Config);

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
