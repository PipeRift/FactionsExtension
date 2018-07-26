// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

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

	virtual void BeginDestroy() override;

	void OnWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS);

#if WITH_EDITOR
	virtual bool CanEditChange(const UProperty* InProperty) const override;

public:

	/**
	* Editor only usage
	* @return all factions as a reference
	*/
	TMap<FName, FFactionInfo>& Internal_GetFactions() { return Factions; }

	/** Gets the member name for the factions */
	static FName GetFactionsPropertyName() { return GET_MEMBER_NAME_CHECKED(UFactionsSettings, Factions); }
#endif
};
