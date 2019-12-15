// Copyright 2015-2019 Piperift. All Rights Reserved.

#pragma once

#include "GenericTeamAgentInterface.h"

#include "Faction.generated.h"

#define NO_FACTION_NAME FName{}

struct FFactionInfo;


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
	FName Name;

public:

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Faction)
	//int32 Id;


	FFaction() : Name(NO_FACTION_NAME) {
	}

	FFaction(FName Name)
		: Name(Name)
	{}

	FFaction(const FGenericTeamId& InTeam);


	/**
	* Find the information of a faction
	* @param Info associated to the faction, if found
	* @return true if the faction was valid and information was found
	*/
	bool GetFactionInfo(FFactionInfo& Info) const;

	/**
	* Replace the information of a faction
	* @param Info to replace the previous one
	* @return true if the faction was found and modified
	*/
	bool SetFactionInfo(const FFactionInfo& NewInfo) const;

	bool IsNone() const;

	/**
	 * Attitude evaluation
	 */
	FORCEINLINE bool IsFriendlyTowards(const FFaction& Other) const {
		return GetAttitudeTowards(Other) == ETeamAttitude::Friendly;
	}
	FORCEINLINE bool IsNeutralTowards(const FFaction& Other) const {
		return GetAttitudeTowards(Other) == ETeamAttitude::Neutral;
	}
	FORCEINLINE bool IsHostileTowards(const FFaction& Other) const {
		return GetAttitudeTowards(Other) == ETeamAttitude::Hostile;
	}

	const ETeamAttitude::Type GetAttitudeTowards(const FFaction& Other) const;

	/**
	 * Operator overloading & Hashes
	 */
	FORCEINLINE bool operator==(const FFaction& Other) const { return Name == Other.Name; }
	FORCEINLINE bool operator!=(const FFaction& Other) const { return !(*this == Other); }

	// Implicit conversion to GenericTeamId
	operator FGenericTeamId() const
	{
		return GetTeam();
	}

	friend uint32 GetTypeHash(const FFaction& InRelation)
	{
		return GetTypeHash(InRelation.Name);
	}

	FName GetIdName() const {
		return Name;
	}

	FString ToString() const {
		return GetIdName().ToString();
	}

	FString GetDisplayName() const;

	const FGenericTeamId GetTeam() const;
};
