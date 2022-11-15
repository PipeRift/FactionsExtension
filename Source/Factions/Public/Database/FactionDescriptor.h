// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include <GenericTeamAgentInterface.h>
#include <Engine/Texture.h>

#include "Faction.h"
#include "FactionDescriptorAsset.h"
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
	FLinearColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cosmetics)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cosmetics)
	TSoftObjectPtr<UTexture> Icon;

	/** Asset containing optional extra fields for a faction.
	 * Create a class from UFactionDescriptorAsset to create your own fields (either in C++ or BPs)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cosmetics, meta = (DisplayThumbnail = false))
	TSoftObjectPtr<UFactionDescriptorAsset> Descriptor;


	FFactionDescriptor(FLinearColor InColor = FColor::Cyan)
		: Color(InColor)
	{}

	FFactionDescriptor(FText DisplayName, FLinearColor InColor = FColor::Cyan)
		: Color(InColor)
		, DisplayName(DisplayName)
	{}
};
