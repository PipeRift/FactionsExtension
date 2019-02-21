// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "Customizations/StringEnumCustomization.h"

#include <DetailWidgetRow.h>
#include <Widgets/Input/SSearchBox.h>
#include <Widgets/Input/SComboButton.h>

#define LOCTEXT_NAMESPACE "FStringEnumCustomization"


void FStringEnumCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	if (CanCustomizeHeader(StructPropertyHandle, HeaderRow, StructCustomizationUtils))
	{
		UpdateItems(false);

		HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MaxDesiredWidth(0.0f)
		.MinDesiredWidth(125.0f)
		[
			SAssignNew(ComboButton, SComboButton)
			.ForegroundColor(this, &FStringEnumCustomization::GetForegroundColor)
			.ToolTipText(this, &FStringEnumCustomization::GetSelectedText)
			.OnGetMenuContent(this, &FStringEnumCustomization::GetListContent)
			.OnComboBoxOpened(this, &FStringEnumCustomization::HandleMenuOpen)
			.ContentPadding(FMargin(2.0f, 2.0f))
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text(this, &FStringEnumCustomization::GetSelectedText)
			]
		];
	}

}


void FStringEnumCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	//Do Nothing
}

TSharedRef<SWidget> FStringEnumCustomization::GetListContent()
{
	SAssignNew(ComboListView, SListView<TSharedPtr<FString> >)
		.ListItemsSource(&CachedItems)
		.OnSelectionChanged(this, &FStringEnumCustomization::OnSelectionChanged)
		.OnGenerateRow(this, &FStringEnumCustomization::HandleComboBoxGenerateWidget)
		.SelectionMode(ESelectionMode::Single);

	// Ensure no filter is applied at the time the menu opens
	OnFilterTextChanged(FText::GetEmpty());

	if (SelectedItem.IsValid())
	{
		ComboListView->SetSelection(SelectedItem);
	}

	return SNew(SVerticalBox)
	+ SVerticalBox::Slot()
	.AutoHeight()
	[
		SAssignNew(SearchBox, SSearchBox)
		.OnTextChanged(this, &FStringEnumCustomization::OnFilterTextChanged)
	]

	+ SVerticalBox::Slot()
	.FillHeight(1.f)
	[
		ComboListView.ToSharedRef()
	];
}

TSharedRef<ITableRow> FStringEnumCustomization::HandleComboBoxGenerateWidget(TSharedPtr<FString> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
	[
		SNew(STextBlock).Text(FText::FromString(*InItem))
	];
}

/** Update the root data on a change of selection */
void FStringEnumCustomization::OnSelectionChanged(TSharedPtr<FString> NewSelectedItem, ESelectInfo::Type SelectInfo)
{
	if (NewSelectedItem.IsValid()) {
		SelectedItem = NewSelectedItem;

		// Close the combo
		ComboButton->SetIsOpen(false);

		//UpdateItems();
		OnItemSelected(*SelectedItem);
	}
}

void FStringEnumCustomization::OnFilterTextChanged(const FText& InFilterText)
{
	FilterText = InFilterText.ToString();
	UpdateItems(true);
}

void FStringEnumCustomization::HandleMenuOpen()
{
	//TODO: Keyboard focus not working
	FSlateApplication::Get().SetKeyboardFocus(SearchBox);
}

/** Display the current column selection */
void FStringEnumCustomization::UpdateItems(bool bRefresh)
{
	TArray<FString> Items;
	GetAllItems(Items);

	CachedItems.Empty();

	//Convert FString to Shared Ptrs and Populate the array
	for (auto It = Items.CreateConstIterator(); It; ++It)
	{
		FString Item = *It;
		if (!Item.IsEmpty() &&
			(FilterText == TEXT("") || Item.Contains(FilterText)))
		{
			TSharedPtr<FString> Name = MakeShareable(new FString(*It));
			CachedItems.Add(Name);
		}
	}

	if (bRefresh && ComboListView.IsValid()) {
		ComboListView->RequestListRefresh();
	}
}

void FStringEnumCustomization::GetAllItems(TArray<FString>& Values) const {
	Values.Add(FString("None"));
}

/** Display the current column selection */
FText FStringEnumCustomization::GetSelectedText() const
{
	return FText::FromString(GetSelectedItem());
}

FSlateColor FStringEnumCustomization::GetForegroundColor() const
{
	return FCoreStyle::Get().GetSlateColor("InvertedForeground");
}


#undef LOCTEXT_NAMESPACE