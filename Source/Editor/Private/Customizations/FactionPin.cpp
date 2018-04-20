// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "FactionPin.h"

#include "KismetEditorUtilities.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"

#include "Faction.h"
#include "FactionsModule.h"
#include "FactionsSettings.h"

 
void SFactionPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	FFactionsModule& Module = FFactionsModule::Get();

    //Bind On Settings Changed event
    Module.OnModifiedSettings().BindRaw(this, &SFactionPin::UpdateItems, false);

    SStringEnumPin::Construct(SStringEnumPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget> SFactionPin::GetDefaultValueWidget()
{
    //Get actual default value
    ParseDefaultValue();

    return SStringEnumPin::GetDefaultValueWidget();
}

void SFactionPin::ParseDefaultValue()
{
    FString IdString = GraphPinObj->GetDefaultAsString();
    if (IdString.StartsWith(TEXT("(")) && IdString.EndsWith(TEXT(")")))
    {
        //Remove ( and )
        IdString = IdString.LeftChop(1);
        IdString = IdString.RightChop(1);

        //Get parameter string value
        IdString.Split("=", NULL, &IdString);
        if (IdString.StartsWith(TEXT("\"")) && IdString.EndsWith(TEXT("\"")))
        {
            IdString = IdString.LeftChop(1);
            IdString = IdString.RightChop(1);
        }
    }

    if (!IdString.IsEmpty())
    {
        FactionDefaultIdValue = FCString::Atoi(*IdString);
    }
	else
	{
		FactionDefaultIdValue = NO_FACTION;
	}
}

void SFactionPin::ApplyDefaultValue()
{
    FString IdStr = FString::FromInt(FactionDefaultIdValue);

    // Set Pin Data
    FString PriorityString;
    if (!IdStr.IsEmpty())
    {
        PriorityString = TEXT("(");
        PriorityString += TEXT("Id=\"");
        PriorityString += IdStr;
        PriorityString += TEXT("\")");
    }
    FString CurrentDefaultValue = GraphPinObj->GetDefaultAsString();
    if (CurrentDefaultValue.IsEmpty())
    {
        CurrentDefaultValue = FString(TEXT(""));
    }
    if (!CurrentDefaultValue.Equals(PriorityString))
    {
        GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, PriorityString);
    }
}


void SFactionPin::GetEnumItems(TArray<FString>& Values)
{
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

void SFactionPin::OnItemSelected(FString Value)
{
	const TArray<FFactionInfo>& AllFactions = GetDefault<UFactionsSettings>()->Factions;

	FName NameValue = FName(*Value);

	const int32 Id = AllFactions.IndexOfByPredicate([NameValue](auto& Info) {
		return Info.Name == NameValue;
	});

	//If Faction not found, Set default value
    if (Id != INDEX_NONE)
        FactionDefaultIdValue = Id;
    else 
        FactionDefaultIdValue = NO_FACTION;

    ApplyDefaultValue();
}

FText SFactionPin::GetSelectedItem() const
{
    //Call parent but don't use it. This is for widget logic
    SStringEnumPin::GetSelectedItem();

	const TArray<FFactionInfo>& AllFactions = GetDefault<UFactionsSettings>()->Factions;

	if (AllFactions.IsValidIndex(FactionDefaultIdValue))
	{
		//Return name with prefix number
		return FText::FromName(AllFactions[FactionDefaultIdValue].Name);
	}
	return FText::FromName(NO_FACTION_NAME);
}
