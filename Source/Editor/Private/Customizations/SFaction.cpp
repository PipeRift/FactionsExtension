// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "SFaction.h"

#include "FactionsSettings.h"
#include "PropertyEditorModule.h"


void SFaction::Construct(const FArguments& InArgs, TSharedRef<IPropertyHandle> _FactionHandle)
{
	OnChange = InArgs._OnChange;
	FactionHandle = _FactionHandle;
	if(FactionHandle.IsValid())
		IdHandle = FactionHandle->GetChildHandle("Id");

	ChildSlot
	[
		SAssignNew(ComboBox, SComboBox<TSharedPtr<FName>>)
		.OptionsSource(&CachedItems)
		.OnGenerateWidget(this, &SFaction::HandleStringEnumComboBoxGenerateWidget)
		.OnComboBoxOpening(this, &SFaction::UpdateItems, false)
		.OnSelectionChanged(this, &SFaction::OnSelectionChanged)
		//.InitiallySelectedItem(GetVariableFactionValue())
		[
			SNew(STextBlock)
			.Text(this, &SFaction::GetSelectedItem)
		]
	];
}

TSharedRef<SWidget> SFaction::HandleStringEnumComboBoxGenerateWidget(const TSharedPtr<FName> Item)
{
	return SNew(STextBlock)
		.Text(FText::FromName(*Item));
}

void SFaction::UpdateItems(bool bRefreshComboBox /*= false*/)
{
	TArray<FName> Names;
	GetFactionNames(Names);

	CachedItems.Empty();

	//Convert FString to Shared Ptrs and Populate the array
	for (auto It = Names.CreateConstIterator(); It; ++It)
	{
		if (It && !It->IsNone())
		{
			CachedItems.Add(MakeShared<FName>(*It));
		}
	}

	if (bRefreshComboBox && ComboBox.IsValid()) {
		ComboBox->RefreshOptions();
	}
}

void SFaction::OnSelectionChanged(const TSharedPtr<FName> SelectedNamePtr, ESelectInfo::Type SelectInfo)
{
	if (!IdHandle.IsValid())
		return;

	if (SelectedNamePtr.IsValid() && !SelectedNamePtr->IsNone())
	{
		FName SelectedName = *SelectedNamePtr;
		const TArray<FFactionInfo>& AllFactions = GetDefault<UFactionsSettings>()->Factions;

		const int32 Id = AllFactions.IndexOfByPredicate([SelectedName](auto& Info) {
			return Info.Name == SelectedName;
		});

		if (Id != INDEX_NONE)
		{
			IdHandle->SetValue(Id);
		}
		else
		{
			//Priority not found. Set default value
			IdHandle->SetValue(NO_FACTION);
		}
	}
	
	UpdateItems(true);
}

FText SFaction::GetSelectedItem() const
{
	if (!IdHandle.IsValid())
		return FText::FromName(NO_FACTION_NAME);

	int32 Id;
	const FPropertyAccess::Result RowResult = IdHandle->GetValue(Id);
	const TArray<FFactionInfo>& AllFactions = GetDefault<UFactionsSettings>()->Factions;

	if (RowResult != FPropertyAccess::MultipleValues)
	{
		if (AllFactions.IsValidIndex(Id))
		{
			//Return name with prefix number
			return FText::FromName(AllFactions[Id].Name);
		}
	}
	return FText::FromName(NO_FACTION_NAME);
}

void SFaction::GetFactionNames(TArray<FName>& Names) const
{
	const UFactionsSettings* Settings = GetDefault<UFactionsSettings>();
	if (!Settings) {
		return;
	}

	for (auto& Info : Settings->Factions)
	{
		Names.Add(Info.Name);
	}
	// Make sure None is at the start
	Names.Remove(NO_FACTION_NAME);
	Names.Insert(NO_FACTION_NAME, 0);
}
