// Copyright 2015-2023 Piperift. All Rights Reserved.

#pragma once

#include "Faction.h"

#include <Engine/Texture.h>
#include <GenericTeamAgentInterface.h>
#include <InstancedStruct.h>

#include "FactionDescriptor.generated.h"


/**
 * Struct containing information about a faction.
 */
USTRUCT(BlueprintType)
struct FACTIONS_API FFactionDescriptor
{
	GENERATED_BODY()

public:
	/** Attitude this faction will have against itself. Relations can override it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Behavior)
	TEnumAsByte<ETeamAttitude::Type> SelfAttitude = ETeamAttitude::Friendly;

	/** Attitude this faction will have against others. Relations can override it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Behavior)
	TEnumAsByte<ETeamAttitude::Type> ExternalAttitude = ETeamAttitude::Neutral;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cosmetics)
	FLinearColor Color = FColor::Cyan;

	UPROPERTY(
		EditAnywhere, BlueprintReadWrite, Category = Cosmetics, meta = (EditCondition = "!bIdAsDisplayName"))
	FText DisplayName;

	// If true, DisplayName is ignored and Id is used instead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cosmetics)
	bool bIdAsDisplayName = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cosmetics)
	TSoftObjectPtr<UTexture> Icon;

	/** Contains optional extra fields for a faction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cosmetics)
	FInstancedStruct Details;
};
