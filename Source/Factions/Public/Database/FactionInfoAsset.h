// Copyright 2015-2019 Piperift. All Rights Reserved.

#pragma once

#include <Engine/DataAsset.h>

#include "Faction.h"
#include "FactionInfoAsset.generated.h"

#define LOCTEXT_NAMESPACE "FactionInfoAsset"


/**
 * Struct containing information about a faction. Static use.
 */
UCLASS(Blueprintable, Abstract)
class FACTIONS_API UFactionInfoAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
};

#undef LOCTEXT_NAMESPACE
