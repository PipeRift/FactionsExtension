// Copyright 2015-2019 Piperift. All Rights Reserved.

#pragma once

#include <GenericTeamAgentInterface.h>
#include <Engine/Texture.h>

#include "Faction.h"
#include "FactionInfoAsset.h"
#include "FactionInfo.generated.h"

#define LOCTEXT_NAMESPACE "FactionInfo"


/**
 * Struct containing information about a faction. Static use.
 */
USTRUCT(BlueprintType)
struct FACTIONS_API FFactionInfo
{
	GENERATED_BODY()

public:
	FFactionInfo(FLinearColor InColor = FColor::Cyan)
		: DisplayName(LOCTEXT("FactionDisplayName", ""))
		, Color(InColor)
		, AttitudeTowardsItself(ETeamAttitude::Friendly)
		, DefaultAttitudeTowardsOthers(ETeamAttitude::Neutral)
		, InfoAsset()
	{}

	FFactionInfo(FText DisplayName, FLinearColor InColor = FColor::Cyan)
		: DisplayName(DisplayName)
		, Color(InColor)
		, AttitudeTowardsItself(ETeamAttitude::Friendly)
		, DefaultAttitudeTowardsOthers(ETeamAttitude::Neutral)
		, InfoAsset()
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

	/** Icon of the faction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Faction)
	TSoftObjectPtr<UTexture> Icon;

	/** Asset containing optional extra fields for a faction.
	 * Create a class from UFactionInfoAsset to create your own fields (either in C++ or BPs)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Faction, meta = (DisplayThumbnail = false))
	TAssetPtr<UFactionInfoAsset> InfoAsset;
};

#undef LOCTEXT_NAMESPACE
