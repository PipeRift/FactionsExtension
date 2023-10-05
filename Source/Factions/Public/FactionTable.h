// Copyright 2015-2023 Piperift. All Rights Reserved.

#pragma once

#include "FactionDescriptor.h"

#include <CoreMinimal.h>
#include <UObject/ObjectMacros.h>

#include "FactionTable.generated.h"


struct FFaction;

/**
 * Contains a list of in-editor editable factions
 */
USTRUCT()
struct FACTIONS_API FFactionTable
{
	GENERATED_BODY()

	UPROPERTY(config, EditAnywhere, Category = "Factions", SaveGame)
	TMap<FName, FFactionDescriptor> List;


	const FFactionDescriptor* GetDescriptor(FFaction Faction) const;
	FFactionDescriptor* GetDescriptor(FFaction Faction);

	int32 Num() const
	{
		return List.Num();
	}
};
