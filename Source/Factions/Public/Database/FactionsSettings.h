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

	UPROPERTY(config, EditAnywhere, Category = Factions, SaveGame)
	FFactionTable FactionList;

public:

	UPROPERTY(config, EditAnywhere, Category = Factions, SaveGame)
	FRelationTable Relations;


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
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

public:

	FFactionTable& GetFactionTable() { return FactionList; }
	const TMap<FName, FFactionInfo>& GetFactionInfos() const { return FactionList.Items; }
};
