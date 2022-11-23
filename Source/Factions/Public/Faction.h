// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include "Faction.generated.h"


/**
 * Represents an existing faction from the database
 */
USTRUCT(BlueprintType)
struct FACTIONS_API FFaction
{
	GENERATED_BODY()

	static inline const FName NoId{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Faction)
	FName Id;


public:
	FFaction(FName Id = NoId) : Id(Id) {}

	bool IsNone() const
	{
		return Id == NoId;
	}

	FName GetId() const
	{
		return Id;
	}

	FString ToString() const
	{
		return GetId().ToString();
	}

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

	friend uint32 GetTypeHash(const FFaction& InRelation)
	{
		return GetTypeHash(InRelation.Id);
	}

	static const FFaction NoFaction()
	{
		return {NoId};
	}
};
