// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "GenericTeamAgentInterface.h"

#include "Faction.h"
#include "FactionInfo.generated.h"

#define LOCTEXT_NAMESPACE "FactionInfo"


/**
 * Struct containing information about a faction. Static use.
 */
USTRUCT(BlueprintType)
struct FACTIONS_API FFactionInfo
{
	GENERATED_USTRUCT_BODY()

public:
	FFactionInfo(FLinearColor InColor = FColor::Cyan)
		: DisplayName(LOCTEXT("FactionDisplayName", ""))
		, Color(FColor::Cyan)
		, AttitudeTowardsItself(ETeamAttitude::Friendly)
		, DefaultAttitudeTowardsOthers(ETeamAttitude::Neutral)
	{}

	FFactionInfo(FText DisplayName, FLinearColor InColor = FColor::Cyan)
		: DisplayName(DisplayName)
		, Color(InColor)
		, AttitudeTowardsItself(ETeamAttitude::Friendly)
		, DefaultAttitudeTowardsOthers(ETeamAttitude::Neutral)
	{}

	/** Name of the faction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Faction)
	FText DisplayName;

	/** Color of the Faction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Faction)
	FLinearColor Color;

	/** Attitude this faction will have against itself. Relations will override it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Faction)
	TEnumAsByte<ETeamAttitude::Type> AttitudeTowardsItself;

	/** Attitude this faction will have against others. Relations will override it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Faction)
	TEnumAsByte<ETeamAttitude::Type> DefaultAttitudeTowardsOthers;
};

#undef LOCTEXT_NAMESPACE
