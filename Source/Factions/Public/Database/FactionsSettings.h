// Copyright 2015-2019 Piperift. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <Engine/World.h>

#include "FactionTable.h"
#include "RelationTable.h"
#include "Reputation/ReputationStep.h"
#include "FactionsSettings.generated.h"


/**
 * Find Custom Config documentation here: wiki.unrealengine.com/CustomSettings
 */
UCLASS(config = Game, defaultconfig)
class FACTIONS_API UFactionsSettings : public UObject
{
	GENERATED_BODY()

protected:

	/** Faction used when an Actor doesn't implement a FactionAgentInterface */
	//UPROPERTY(config, EditAnywhere, Category = Custom)
	//FFaction DefaultFaction;

	// #TODO: Move deprecated factions into FactionList on PostLoad
	UPROPERTY(config, SaveGame)
	TMap<FName, FFactionInfo> Factions_DEPRECATED;

	UPROPERTY(config, EditAnywhere, Category = Factions, SaveGame)
	FFactionTable FactionList;

public:

	UPROPERTY(config, EditAnywhere, Category = Factions, SaveGame)
	FRelationTable Relations;


	/** Begin Reputation */
	UPROPERTY(config, EditAnywhere, Category = Reputation, SaveGame)
	FReputationStep DefaultStep;

	UPROPERTY(config, EditAnywhere, Category = Reputation, SaveGame)
	TArray<FReputationStep> PositiveSteps;

	UPROPERTY(config, EditAnywhere, Category = Reputation, SaveGame)
	TArray<FReputationStep> NegativeSteps;
	/** End Reputation */


public:

	UFactionsSettings();

	bool Internal_RegistryRelation(const FFactionRelation& Relation);
	bool Internal_UnregistryRelation(const FFactionRelation& Relation);

	const FFactionRelation* FindRelation(const FFaction& A, const FFaction& B) const
	{
		return Relations.GetRelations().Find({ A, B });
	}

protected:

	virtual void BeginDestroy() override;

	void OnWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS);

#if WITH_EDITOR
	virtual bool CanEditChange(const UProperty* InProperty) const override;

public:

	bool UpdateDeprecations();

	/** Gets the member name for the factions */
	static FName GetFactionsPropertyName() { return GET_MEMBER_NAME_CHECKED(UFactionsSettings, Factions_DEPRECATED); }
#endif

public:

	FFactionTable& GetFactionTable() { return FactionList; }
	const TMap<FName, FFactionInfo>& GetFactionInfos() const { return FactionList.Items; }
};
