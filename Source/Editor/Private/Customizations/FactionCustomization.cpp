// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "FactionCustomization.h"

#include "FactionsModule.h"

#include "Faction.h"
#include "FactionsSettings.h"

#define LOCTEXT_NAMESPACE "FFactionCustomization"


bool FFactionCustomization::CanCustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
    StructHandle = StructPropertyHandle;
    IdHandle = StructPropertyHandle->GetChildHandle("Id");

    if (IdHandle->IsValidHandle())
	{
		FFactionsModule& Module = FFactionsModule::Get();
		//Bind On Settings Changed event
        Module.OnModifiedSettings().BindRaw(this, &FFactionCustomization::UpdateItems, true);
        return true;
    }
    return false;
}

void FFactionCustomization::GetEnumItems(TArray<FString>& Values) const {
    const UFactionsSettings* Settings = GetDefault<UFactionsSettings>();
    if (!Settings) {
        return;
    }

    for (auto& Info : Settings->Factions)
    {
        Values.Add(Info.Name.ToString());
    }
    // Make sure None is at the start
    Values.Remove(NO_FACTION_NAME.ToString());
    Values.Insert(NO_FACTION_NAME.ToString(), 0);
}

void FFactionCustomization::OnItemSelected(FString Value)
{
    const TArray<FFactionInfo>& AllFactions = GetDefault<UFactionsSettings>()->Factions;

	FName NameValue = FName(*Value);

    const int32 Id = AllFactions.IndexOfByPredicate([NameValue](auto& Info) {
		return Info.Name == NameValue;
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

/** Display the current column selection */
FText FFactionCustomization::GetSelectedItem() const
{
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
        return FText::FromName(NO_FACTION_NAME);
    }
    return LOCTEXT("MultipleValues", "Multiple Values");
}

#undef LOCTEXT_NAMESPACE
