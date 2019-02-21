// Copyright 2015-2019 Piperift. All Rights Reserved.

#pragma once

#include "GenericTeamAgentInterface.h"

#include "Faction.h"
#include "FactionRelation.generated.h"

#define LOCTEXT_NAMESPACE "FactionRelation"


/**
 * Defines a relation between factions
*/
USTRUCT(BlueprintType)
struct FACTIONS_API FFactionRelation
{
	GENERATED_USTRUCT_BODY()

	FFactionRelation()
		: FactionA(FFaction::NoFaction)
		, FactionB(FFaction::NoFaction)
		, Attitude(ETeamAttitude::Neutral)
	{}

	FFactionRelation(FFaction A, FFaction B, ETeamAttitude::Type InAttitude = ETeamAttitude::Neutral)
		: FactionA(A), FactionB(B), Attitude(InAttitude)
	{}

	/** First Faction of the relation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Relation, meta = (DisplayName = "A"))
	FFaction FactionA;

	/** Second Faction of the relation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Relation, meta = (DisplayName = "B"))
	FFaction FactionB;

	/** Defines how this two factions will react to each other */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Relation)
	TEnumAsByte<ETeamAttitude::Type> Attitude;


	bool IsValid() const
	{
		// Factions are the same or both are invalid
		return FactionA == FactionB || (!FactionA.IsNone() && !FactionB.IsNone());
	}

	FORCEINLINE bool operator==(const FFactionRelation& Other) const {
		return (FactionA == Other.FactionA && FactionB == Other.FactionB) ||
			   (FactionA == Other.FactionB && FactionB == Other.FactionA);
	}
	FORCEINLINE bool operator!=(const FFactionRelation& Other) const { return !(*this == Other); }

	friend uint32 GetTypeHash(const FFactionRelation& InRelation)
	{
		return GetTypeHash(InRelation.FactionA) ^ GetTypeHash(InRelation.FactionB);
	}
};

#undef LOCTEXT_NAMESPACE
