// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include <GenericTeamAgentInterface.h>

#include "FactionBehavior.generated.h"


/**
 * Defined the behavior of a faction after it has been backed form a FactionTable
 */
USTRUCT()
struct FACTIONS_API FFactionBehavior
{
	GENERATED_BODY()

	/** Attitude this faction will have against itself. Relations can override it. */
	UPROPERTY()
	TEnumAsByte<ETeamAttitude::Type> SelfAttitude = ETeamAttitude::Friendly;

	/** Attitude this faction will have against others. Relations can override it. */
	UPROPERTY()
	TEnumAsByte<ETeamAttitude::Type> ExternalAttitude = ETeamAttitude::Neutral;

	// TODO: Point to relations here
};
