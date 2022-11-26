// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include "Faction.h"
#include "GenericTeamAgentInterface.h"

#include "FactionRelation.generated.h"


#define LOCTEXT_NAMESPACE "FactionRelation"


/**
 * Defines a relation between factions
 */
USTRUCT(BlueprintType)
struct FACTIONS_API FFactionRelation
{
	GENERATED_BODY()

	FFactionRelation()
		: Source(FFaction::NoFaction())
		, Target(FFaction::NoFaction())
		, Attitude(ETeamAttitude::Neutral)
	{}

	FFactionRelation(FFaction Source, FFaction Target, ETeamAttitude::Type InAttitude = ETeamAttitude::Neutral)
		: Source(Source)
		, Target(Target)
		, Attitude(InAttitude)
	{}

	/** First Faction of the relation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Relation, meta = (DisplayName = "A"))
	FFaction Source;

	/** Second Faction of the relation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Relation, meta = (DisplayName = "B"))
	FFaction Target;

	/** Defines how this two factions will react to each other */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Relation)
	TEnumAsByte<ETeamAttitude::Type> Attitude;

	/** Does this relation define the attitude from Source to Target only, or Target to Source as well? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Relation)
	bool bBidirectional = true;


	bool IsValid() const
	{
		// Factions are the same or both are invalid
		return Source == Target || (!Source.IsNone() && !Target.IsNone());
	}

	FORCEINLINE bool operator==(const FFactionRelation& Other) const
	{
		return (Source == Other.Source && Target == Other.Target) ||
			(bBidirectional && Source == Other.Target && Target == Other.Source);
	}
	FORCEINLINE bool operator!=(const FFactionRelation& Other) const
	{
		return !(*this == Other);
	}

	friend uint32 GetTypeHash(const FFactionRelation& InRelation)
	{
		return GetTypeHash(InRelation.Source) ^ GetTypeHash(InRelation.Target);
	}
};

#undef LOCTEXT_NAMESPACE
