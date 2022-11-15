// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include "FactionDescriptor.h"

#include <CoreMinimal.h>
#include <UObject/ObjectMacros.h>

#include "FactionTable.generated.h"


struct FFaction;

USTRUCT()
struct FACTIONS_API FFactionTable
{
	GENERATED_BODY()

	UPROPERTY(config, EditAnywhere, Category = "Factions", SaveGame)
	TMap<FName, FFactionDescriptor> Descriptors;


	FFaction AddFaction(FName Id, const FFactionDescriptor& Descriptor);
	void RemoveFaction(FFaction Faction);

	const FFactionDescriptor* GetDescriptor(FFaction Faction) const;
	FFactionDescriptor* GetDescriptor(FFaction Faction);
};
