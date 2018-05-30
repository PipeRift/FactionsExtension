// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "FactionCustomization.h"

#include "FactionsModule.h"

#include "Faction.h"
#include "FactionsSettings.h"

#define LOCTEXT_NAMESPACE "FFactionCustomization"


bool FFactionCustomization::CanCustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;
	NameHandle = StructPropertyHandle->GetChildHandle("Name");

	if (NameHandle->IsValidHandle())
	{
		FFactionsModule& Module = FFactionsModule::Get();
		//Bind On Settings Changed event
		Module.OnModifiedSettings().BindRaw(this, &FFactionCustomization::UpdateItems, true);
		return true;
	}
	return false;
}

void FFactionCustomization::GetAllItems(TArray<FString>& Values) const {
	const UFactionsSettings* Settings = GetDefault<UFactionsSettings>();
	if (!Settings) {
		return;
	}

	for (const auto& KeyValue : Settings->Factions)
	{
		Values.Add(KeyValue.Key.ToString());
	}
	// Make sure None is at the start
	Values.Remove(NO_FACTION_NAME.ToString());
	Values.Insert(NO_FACTION_NAME.ToString(), 0);
}

void FFactionCustomization::OnItemSelected(FString Value)
{
	const TMap<FName, FFactionInfo>& AllFactions = GetDefault<UFactionsSettings>()->Factions;

	FName NameValue = FName(*Value);

	if (NameValue != NO_FACTION_NAME && AllFactions.Contains(NameValue))
	{
		NameHandle->SetValue(NameValue);
	}
	else
	{
		//Priority not found. Set default value
		NameHandle->SetValue(NO_FACTION_NAME);
	}
}

/** Display the current column selection */
FText FFactionCustomization::GetSelectedText() const
{
	FName Name;
	const FPropertyAccess::Result RowResult = NameHandle->GetValue(Name);
	const TMap<FName, FFactionInfo>& AllFactions = GetDefault<UFactionsSettings>()->Factions;

	if (RowResult != FPropertyAccess::MultipleValues)
	{
		const FFactionInfo* Info = AllFactions.Find(Name);
		if (Info)
		{
			return FText::FromName(Name);
		}
		return FText::FromName(NO_FACTION_NAME);
	}
	return LOCTEXT("MultipleValues", "Multiple Values");
}

#undef LOCTEXT_NAMESPACE
