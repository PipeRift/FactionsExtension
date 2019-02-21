// Copyright 2015-2019 Piperift. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <UObject/ObjectMacros.h>

#include "FactionInfo.h"
#include "FactionTable.generated.h"


struct FFaction;

USTRUCT()
struct FACTIONS_API FFactionTable
{
	GENERATED_BODY()


	UPROPERTY(config, EditAnywhere, Category = "Factions", SaveGame)
	TMap<FName, FFactionInfo> Items;


	FFaction RegistryFaction(FName Name, FFactionInfo Info);
	bool UnregistryFaction(FFaction Faction);

	bool SetInfo(FFaction Faction, const FFactionInfo& NewInfo);
};


