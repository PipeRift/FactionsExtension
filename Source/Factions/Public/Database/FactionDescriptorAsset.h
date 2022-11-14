// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include <Engine/DataAsset.h>

#include "Faction.h"
#include "FactionDescriptorAsset.generated.h"

#define LOCTEXT_NAMESPACE "FactionDescriptorAsset"


/**
 * Struct containing information about a faction. Static use.
 */
UCLASS(Blueprintable, Abstract)
class FACTIONS_API UFactionDescriptorAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
};

#undef LOCTEXT_NAMESPACE
