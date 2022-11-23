// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "Customizations/FactionPin.h"

#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"

#include "Faction.h"
#include "FactionsModule.h"
#include "FactionsSubsystem.h"


void SFactionPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget> SFactionPin::GetDefaultValueWidget()
{
	ParseDefaultValue();

	return SNew(SBox)
	.MaxDesiredHeight(20.f)
	.Visibility( this, &SGraphPin::GetDefaultValueVisibility )
	[
		SNew(SFaction)
		.ContentPadding(0.f)
		.Faction_Lambda([this]() {
			return FFaction{FactionDefaultNameValue};
		})
		.OnFactionSelected_Lambda([this](FFaction Faction, ESelectInfo::Type)
		{
			FactionDefaultNameValue = Faction.GetId();
			ApplyDefaultValue();
		})
	];
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
		FactionDefaultNameValue = FFaction::NoId;
	}
}

void SFactionPin::ApplyDefaultValue()
{
	// Set Pin Data
	FString PriorityString;
	if (!FactionDefaultNameValue.IsNone())
	{
		PriorityString = TEXT("(");
		PriorityString += TEXT("Id=\"");
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
