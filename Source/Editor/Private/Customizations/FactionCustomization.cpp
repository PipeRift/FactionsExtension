// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "AIExtensionEditorPrivatePCH.h"

#include "AIExtensionModule.h"

#include "Faction.h"
#include "AIExtensionSettings.h"

#include "FactionCustomization.h"

#define LOCTEXT_NAMESPACE "FFactionCustomization"

bool FFactionCustomization::CanCustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
    StructHandle = StructPropertyHandle;
    IdHandle = StructPropertyHandle->GetChildHandle("Id");

    if (IdHandle->IsValidHandle())
	{
		//FAIExtensionModule& Module = FAIExtensionModule::GetInstance();
		//Bind On Settings Changed event
        //Module->OnModifiedSettings().BindRaw(this, &FFactionCustomization::UpdateItems);
        return true;
    }
    return false;
}

void FFactionCustomization::GetEnumItems(TArray<FString>& Values) const {
    const UAIExtensionSettings* Settings = GetDefault<UAIExtensionSettings>();
    if (!Settings) {
        return;
    }

    for (auto& Info : Settings->Factions)
    {
        Values.Add(Info.Name);
    }
    // Make sure None is at the start
    Values.Remove(NO_FACTION_NAME);
    Values.Insert(NO_FACTION_NAME, 0);
}

void FFactionCustomization::OnItemSelected(FString Value) {

    const TArray<FFactionInfo>& AllFactions = GetDefault<UAIExtensionSettings>()->Factions;

    const int32 Id = AllFactions.IndexOfByPredicate([Value](auto& Info) {
        return Info.Name == Value;
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
    const TArray<FFactionInfo>& AllFactions = GetDefault<UAIExtensionSettings>()->Factions;

    if (RowResult != FPropertyAccess::MultipleValues)
    {
        if (AllFactions.IsValidIndex(Id))
        {
            //Return name with prefix number
            return FText::FromString(AllFactions[Id].Name);
        }
        return FText::FromString(NO_FACTION_NAME);
    }
    return LOCTEXT("MultipleValues", "Multiple Values");
}

#undef LOCTEXT_NAMESPACE
