// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <Engine/World.h>
#include <Subsystems/GameInstanceSubsystem.h>

#include "FactionTable.h"
#include "RelationTable.h"
#include "Reputation/ReputationStep.h"
#include "FactionsSubsystem.generated.h"


/**
 * The FactionsSubsystem contains the global registry of factions and the attitudes between them
 */
UCLASS(ClassGroup = FactionsExtension, config = Game, defaultconfig, meta = (DisplayName = "Factions"))
class FACTIONS_API UFactionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:

	UPROPERTY(config, EditAnywhere, SaveGame)
	FFactionTable Factions;

	UPROPERTY(config, EditAnywhere, SaveGame)
	FRelationTable Relations;

	//UPROPERTY(Transient)
	//TMap<FName, FFactionBehavior> BakedFactions;

	//UPROPERTY(Transient)
	//TArray<FFactionRelation> BakedRelations;


public:

	UFactionsSubsystem();

	bool Internal_AddRelation(const FFactionRelation& Relation);
	bool Internal_RemoveRelation(const FFactionRelation& Relation);

	const FFactionRelation* FindRelation(const FFaction& A, const FFaction& B) const
	{
		return Relations.GetRelations().Find({ A, B });
	}

	FFactionTable& GetFactions() { return Factions; }
	const FFactionTable& GetFactions() const { return Factions; }

	static UFactionsSubsystem* Get(const UObject* ContextObject);


	/** BLUEPRINT ONLY API */



	/** BLUEPRINTS & C++ API */

	/**
	 * Find the information of a faction
	 * @param Faction to search for
	 * @param Descriptor of the faction, if found
	 * @return true if the faction was valid and information was found
	 */
	UFUNCTION(BlueprintPure, Category = Factions)
	bool GetFactionDescriptor(const FFaction Faction, FFactionDescriptor& Descriptor)
	{
		if (auto* Found = GetFactions().GetDescriptor(Faction))
		{
			Descriptor = *Found;
			return true;
		}
		return false;
	}

	/**
	 * Replace the information of an existing faction
	 * @param Faction to change
	 * @param Descriptor to assign
	 * @return true if the faction was found and modified
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	bool SetFactionDescriptor(const FFaction Faction, const FFactionDescriptor& Descriptor)
	{
		if (auto* Found = GetFactions().GetDescriptor(Faction))
		{
			*Found = Descriptor;
			return true;
		}
		return false;
	}

protected:

	virtual void BeginDestroy() override;

	void OnWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS);

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif
};


inline UFactionsSubsystem* UFactionsSubsystem::Get(const UObject* ContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		return UGameInstance::GetSubsystem<UFactionsSubsystem>(World->GetGameInstance());
	}
	return nullptr;
}
