// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"

#include <Engine/World.h>

#include "FactionInfo.h"
#include "RelationDatabase.h"
#include "FactionsSettings.generated.h"


/**
 * Find Custom Config documentation here: wiki.unrealengine.com/CustomSettings
 */
UCLASS(config = Game, defaultconfig)
class FACTIONS_API UFactionsSettings : public UObject
{
	GENERATED_BODY()

public:

	UFactionsSettings();

	/** Faction used when an Actor doesn't implement a FactionAgentInterface */
	//UPROPERTY(config, EditAnywhere, Category = Custom)
	//FFaction DefaultFaction;

	UPROPERTY(config, EditAnywhere, Category = Factions, SaveGame)
	TMap<FName, FFactionInfo> Factions;

	UPROPERTY(config, EditAnywhere, Category = Relations, SaveGame)
	FRelationDatabase Relations;


public:

	bool Internal_RegistryFaction(const FName& Name, const FFactionInfo& FactionInfo);
	bool Internal_UnregistryFaction(FFaction Faction);

	bool Internal_RegistryRelation(const FFactionRelation& Relation);
	bool Internal_UnregistryRelation(const FFactionRelation& Relation);

	const FFactionRelation* FindRelation(const FFaction& A, const FFaction& B) const
	{
		return Relations.GetRelations().Find({ A, B });
	}

protected:

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual bool CanEditChange(const UProperty* InProperty) const override;

	void SanitizeRelations(EPropertyChangeType::Type ChangeType, int32 RelationIndex = INDEX_NONE);
#endif

	virtual void BeginDestroy() override;

	void OnWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS);
};
