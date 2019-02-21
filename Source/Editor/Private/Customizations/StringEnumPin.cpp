// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "Customizations/StringEnumPin.h"

#include <Kismet2/KismetEditorUtilities.h>
#include <EdGraph/EdGraphPin.h>
#include <EdGraph/EdGraphSchema.h>


void SStringEnumPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget> SStringEnumPin::GetDefaultValueWidget()
{
	UpdateItems();

	return SNew(SBox)
	.MaxDesiredHeight(20.f)
	[
		SAssignNew(ComboBox, SComboBox<TSharedPtr<FString>>)
		.ContentPadding(0.f)
		.OptionsSource(&CachedItems)
		.OnGenerateWidget(this, &SStringEnumPin::HandleStringEnumComboBoxGenerateWidget)
		.OnSelectionChanged(this, &SStringEnumPin::OnSelectionChanged)
		//.InitiallySelectedItem(GetVariablFFactionValue())
		[
			SNew(STextBlock)
			.Text(this, &SStringEnumPin::GetSelectedItem)
		]
	];
}

TSharedRef<SWidget> SStringEnumPin::HandleStringEnumComboBoxGenerateWidget(TSharedPtr<FString> Item)
{
	return SNew(STextBlock)
		.Text(FText::FromString(*Item));
}

/** Update the root data on a change of selection */
void SStringEnumPin::OnSelectionChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
{
	if (SelectedItem.IsValid()) {
		//UpdateItems();
		OnItemSelected(*SelectedItem);
	}
}

void SStringEnumPin::UpdateItems(bool bRefresh)
{
	TArray<FString> Items;
	GetEnumItems(Items);

	CachedItems.Empty();

	//Convert FString to Shared Ptrs and Populate the array
	for (auto It = Items.CreateConstIterator(); It; ++It)
	{
		if (!(*It).IsEmpty())
		{
			TSharedPtr<FString> Name = MakeShareable(new FString(*It));
			CachedItems.Add(Name);
		}
	}

	if (bRefresh && ComboBox.IsValid()) {
		ComboBox->RefreshOptions();
	}
}

void SStringEnumPin::GetEnumItems(TArray<FString>& Values)
{
	Values.Add(FString("None"));
}

FText SStringEnumPin::GetSelectedItem() const
{
	if (ComboBox.IsValid()) {
		ComboBox->SetVisibility(IsConnected() ? EVisibility::Hidden : EVisibility::Visible);
	}

	return FText::FromString("None");
}
