// Copyright 2015-2019 Piperift. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "UObject/ScriptMacros.h"
#include "Faction.h"

#include "ReputationComponent.generated.h"


USTRUCT()
struct FACTIONS_API FReputationItem
{
	GENERATED_BODY()

	UPROPERTY()
	FFaction Faction;

	UPROPERTY()
	int32 Value;

	UPROPERTY()
	int32 MaxValue;
};


UCLASS(ClassGroup = Factions, meta = (BlueprintSpawnableComponent))
class FACTIONS_API UReputationComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	TArray<FReputationItem> reputations;

public:

	UReputationComponent() : Super() {}


	UFUNCTION(BlueprintCallable, Category = "Reputation|Modification")
	void SetReputation(FFaction Faction, int32 Amount) {}

	UFUNCTION(BlueprintCallable, Category = "Reputation|Modification")
	void SetReputationToStep(FFaction Faction, FName Step) {}

	UFUNCTION(BlueprintCallable, Category = "Reputation|Modification")
	void AddReputation(FFaction Faction, int32 Amount) {}
};


