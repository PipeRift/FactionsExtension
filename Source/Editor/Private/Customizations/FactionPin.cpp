// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "Customizations/FactionPin.h"

#include "Kismet2/KismetEditorUtilities.h"
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
	FString NameString = GraphPinObj->GetDefaultAsString();
	if (NameString.StartsWith(TEXT("(")) && NameString.EndsWith(TEXT(")")))
	{
		//Remove ( and )
		NameString = NameString.LeftChop(1);
		NameString = NameString.RightChop(1);

		//Get parameter string value
		NameString.Split("=", nullptr, &NameString);
		if (NameString.StartsWith(TEXT("\"")) && NameString.EndsWith(TEXT("\"")))
		{
			NameString = NameString.LeftChop(1);
			NameString = NameString.RightChop(1);
		}
	}

	if (!NameString.IsEmpty())
	{
		FactionDefaultNameValue = FName(*NameString);
	}
	else
	{
		FactionDefaultNameValue = NO_FACTION_NAME;
	}
}

void SFactionPin::ApplyDefaultValue()
{
	// Set Pin Data
	FString PriorityString;
	if (!FactionDefaultNameValue.IsNone())
	{
		PriorityString = TEXT("(");
		PriorityString += TEXT("Name=\"");
		PriorityString += FactionDefaultNameValue.ToString();
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
	for (const auto& KeyValue : GetDefault<UFactionsSettings>()->GetFactionInfos())
	{
		Values.Add(KeyValue.Key.ToString());
	}
	// Make sure None is at the start
	Values.Remove(NO_FACTION_NAME.ToString());
	Values.Insert(NO_FACTION_NAME.ToString(), 0);
}

void SFactionPin::OnItemSelected(FString Value)
{
	const TMap<FName, FFactionInfo>& AllFactions = GetDefault<UFactionsSettings>()->GetFactionInfos();

	FName NameValue = FName(*Value);

	//If Faction not found, Set default value
	if (NameValue != NO_FACTION_NAME && AllFactions.Contains(NameValue))
		FactionDefaultNameValue = NameValue;
	else
		FactionDefaultNameValue = NO_FACTION_NAME;

	ApplyDefaultValue();
}

FText SFactionPin::GetSelectedItem() const
{
	//Call parent but don't use it. This is for widget logic
	SStringEnumPin::GetSelectedItem();

	const auto& Factions = GetDefault<UFactionsSettings>()->GetFactionInfos();

	if (Factions.Contains(FactionDefaultNameValue))
	{
		//Return name with prefix number
		return FText::FromName(FactionDefaultNameValue);
	}
	return FText::FromName(NO_FACTION_NAME);
}
