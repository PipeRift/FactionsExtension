// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include <GenericTeamAgentInterface.h>

#include "FactionBehavior.generated.h"


/**
 * Struct defining the behavior of a faction
 */
USTRUCT(BlueprintType)
struct FACTIONS_API FFactionBehavior
{
	GENERATED_BODY()

	/** Attitude this faction will have against itself. Relations can override it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETeamAttitude::Type> SelfAttitude = ETeamAttitude::Friendly;

	/** Attitude this faction will have against others. Relations can override it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETeamAttitude::Type> DefaultAttitude = ETeamAttitude::Neutral;
};
