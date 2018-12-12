// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "GenericTeamAgentInterface.h"

#include "Faction.h"
#include "CustomFactionInfo.generated.h"

#define LOCTEXT_NAMESPACE "FactionInfo"


/**
 * Struct containing information about a faction. Static use.
 */
UCLASS(Blueprintable, EditInlineNew)
class FACTIONS_API UCustomFactionInfo : public UObject
{
	GENERATED_BODY()
};

#undef LOCTEXT_NAMESPACE
