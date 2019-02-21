// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "Customizations/SStringEnum.h"
#include <PropertyEditorModule.h>

#include "FactionsSettings.h"


void SStringEnum::Construct(const FArguments& InArgs, TSharedRef<IPropertyHandle> _FactionHandle)
{
	OnChange = InArgs._OnChange;
	FactionHandle = _FactionHandle;
	if(FactionHandle.IsValid())
		NameHandle = FactionHandle->GetChildHandle("Name");

	ChildSlot
	[
		SAssignNew(ComboBox, SComboBox<TSharedPtr<FName>>)
		.OptionsSource(&CachedItems)
		.OnGenerateWidget(this, &SStringEnum::HandleStringEnumComboBoxGenerateWidget)
		.OnComboBoxOpening(this, &SStringEnum::UpdateItems, false)
		.OnSelectionChanged(this, &SStringEnum::OnSelectionChanged)
		//.InitiallySelectedItem(GetVariableFactionValue())
		[
			SNew(STextBlock)
			.Text(this, &SStringEnum::GetSelectedItem)
		]
	];
}

TSharedRef<SWidget> SStringEnum::HandleStringEnumComboBoxGenerateWidget(const TSharedPtr<FName> Item)
{
	return SNew(STextBlock)
		.Text(FText::FromName(*Item));
}

void SStringEnum::UpdateItems(bool bRefreshComboBox /*= false*/)
{
	TArray<FName> Names;
	GetFactionNames(Names);

	CachedItems.Empty();

	//Convert FString to Shared Ptrs and Populate the array
	for (auto It = Names.CreateConstIterator(); It; ++It)
	{
		if (It)
		{
			CachedItems.Add(MakeShared<FName>(*It));
		}
	}

	if (bRefreshComboBox && ComboBox.IsValid()) {
		ComboBox->RefreshOptions();
	}
}

void SStringEnum::OnSelectionChanged(const TSharedPtr<FName> SelectedNamePtr, ESelectInfo::Type SelectInfo)
{
	if (!NameHandle.IsValid())
		return;

	if (SelectedNamePtr.IsValid())
	{
		FName SelectedName = *SelectedNamePtr;
		const TMap<FName, FFactionInfo>& AllFactions = GetDefault<UFactionsSettings>()->GetFactionInfos();

		if (SelectedName != NO_FACTION_NAME && AllFactions.Contains(SelectedName))
		{
			NameHandle->SetValue(SelectedName);
		}
		else
		{
			//Priority not found. Set default value
			NameHandle->SetValue(NO_FACTION_NAME);
		}
	}

	UpdateItems(true);
}

FText SStringEnum::GetSelectedItem() const
{
	if (!NameHandle.IsValid())
		return FText::FromName(NO_FACTION_NAME);

	FName Name;
	const FPropertyAccess::Result RowResult = NameHandle->GetValue(Name);
	const TMap<FName, FFactionInfo>& AllFactions = GetDefault<UFactionsSettings>()->GetFactionInfos();

	if (RowResult != FPropertyAccess::MultipleValues)
	{
		if (AllFactions.Contains(Name))
		{
			//Return name with prefix number
			return FText::FromName(Name);
		}
	}
	return FText::FromName(NO_FACTION_NAME);
}

void SStringEnum::GetFactionNames(TArray<FName>& Names) const
{
	const UFactionsSettings* Settings = GetDefault<UFactionsSettings>();
	if (!Settings) {
		return;
	}

	for (const auto& KeyValue : Settings->GetFactionInfos())
	{
		Names.Add(KeyValue.Key);
	}
	// Make sure None is at the start
	Names.Remove(NO_FACTION_NAME);
	Names.Insert(NO_FACTION_NAME, 0);
}
