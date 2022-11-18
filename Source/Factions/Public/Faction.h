// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include "GenericTeamAgentInterface.h"

#include "Faction.generated.h"


#define NO_FACTION_NAME \
	FName {}


/**
 * Represents an existing faction from the database
 */
USTRUCT(BlueprintType)
struct FACTIONS_API FFaction
{
	GENERATED_BODY()

	static const FFaction NoFaction;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Faction)
	FName Id;


public:
	FFaction() : Id(NO_FACTION_NAME) {}

	FFaction(FName Id) : Id(Id) {}

	FFaction(const FGenericTeamId& InTeam);

	bool IsNone() const;

	/**
	 * Operator overloading & Hashes
	 */
	FORCEINLINE bool operator==(const FFaction& Other) const
	{
		return Id == Other.Id;
	}
	FORCEINLINE bool operator!=(const FFaction& Other) const
	{
		return !(*this == Other);
	}

	// Implicit conversion to GenericTeamId
	operator FGenericTeamId() const
	{
		return GetTeam();
	}

	friend uint32 GetTypeHash(const FFaction& InRelation)
	{
		return GetTypeHash(InRelation.Id);
	}

	FName GetId() const
	{
		return Id;
	}

	FString ToString() const
	{
		return GetId().ToString();
	}

	const FGenericTeamId GetTeam() const;
};
