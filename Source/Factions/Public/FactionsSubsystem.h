// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include "FactionAgentInterface.h"
#include "FactionTable.h"
#include "RelationTable.h"

#include <CoreMinimal.h>
#include <Engine/World.h>
#include <Subsystems/GameInstanceSubsystem.h>

#include "FactionsSubsystem.generated.h"


UENUM()
enum class EFactionTestMode : uint8
{
	IsSame,
	IsFriendly,
	IsNeutral,
	IsHostile
};


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

	// UPROPERTY(Transient)
	// TMap<FName, FFactionBehavior> BakedFactions;

	// UPROPERTY(Transient)
	// TArray<FFactionRelation> BakedRelations;


public:
	UFactionsSubsystem();


	const FFactionRelation* FindRelation(const FFaction& A, const FFaction& B) const
	{
		return Relations.GetRelations().Find({A, B});
	}

	FFactionTable& GetFactions()
	{
		return Factions;
	}
	const FFactionTable& GetFactions() const
	{
		return Factions;
	}

	static UFactionsSubsystem* Get(const UObject* ContextObject);

	const FFactionDescriptor* GetDescriptor(FFaction Faction) const;


	/** BLUEPRINTS & C++ API */

	/** @return Attitude of Source towards Target */
	UFUNCTION(BlueprintPure, Category = Factions)
	TEnumAsByte<ETeamAttitude::Type> GetAttitude(const FFaction Source, const FFaction Target) const;

	/** @return Attitude of Source's faction towards Target's faction */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DefaultToSelf = "Source"))
	TEnumAsByte<ETeamAttitude::Type> GetActorAttitude(const AActor* Source, const AActor* Target) const;

	/** @return true if Source is Hostile to Target */
	UFUNCTION(BlueprintPure, Category = Factions)
	bool IsHostile(const FFaction Source, const FFaction Target) const;

	/** @return true if Source is Friendly to Target */
	UFUNCTION(BlueprintPure, Category = Factions)
	bool IsFriendly(const FFaction Source, const FFaction Target) const;

	/** @return true if Source is Neutral to Target */
	UFUNCTION(BlueprintPure, Category = Factions)
	bool IsNeutral(const FFaction Source, const FFaction Target) const;

	/** @return true if Source is Hostile to Target */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DefaultToSelf = "Source"))
	bool IsActorHostile(const AActor* Source, const AActor* Target);

	/** @return true if Source is Friendly to Target */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DefaultToSelf = "Source"))
	bool IsActorFriendly(const AActor* Source, const AActor* Target);

	/** @return true if One is Neutral to Target */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DefaultToSelf = "Source"))
	bool IsActorNeutral(const AActor* Source, const AActor* Target);

	UFUNCTION(BlueprintPure, Category = Factions)
	FString GetDisplayName(const FFaction Faction) const;

	/**
	 * Adds a faction and sets its descriptor unless it already existed.
	 * @param Descriptor containing information about the faction
	 * @return the faction created
	 * @see EmplaceFaction, RemoveFaction
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	FFaction AddFaction(const FName& Id, const FFactionDescriptor& Descriptor);

	/**
	 * Adds a faction and sets its descriptor even if it already existed.
	 * @param Descriptor containing information about the faction
	 * @return the faction created or found
	 * @see AddFaction, RemoveFaction
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	FFaction EmplaceFaction(const FName& Id, FFactionDescriptor Descriptor);

	/**
	 * Remove a faction
	 * @param Faction to remove
	 * @return true if the faction was removed
	 * @see AddFaction, EmplaceFaction
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	void RemoveFaction(FFaction Faction);

	/**
	 * Add a relation between two factions
	 * @param Relation to be added
	 * @return true if the relation was registered, false if the two factions were the same or invalid.
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	bool AddRelation(const FFactionRelation& Relation);

	/**
	 * Remove a relation between two factions
	 * @param Relation to be removed
	 * @return true if the relation was unregistered
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	bool RemoveRelation(const FFactionRelation& Relation);

	/**
	 * Get all actors in the world with a certain faction
	 * @param Faction to check actors for
	 * @param Comparison relation between the actor and the faction
	 * @param ActorClass limits the actors to be found by that class
	 * @return true if operation was successful and the array of actors
	 */
	UFUNCTION(BlueprintCallable, Category = Factions,
		meta = (AdvancedDisplay = "ActorClass", DeterminesOutputType = "ActorClass",
			DynamicOutputParam = "OutActors"))
	bool GetActorsByFaction(const FFaction Faction, TArray<AActor*>& OutActors,
		TSubclassOf<AActor> ActorClass, EFactionTestMode Comparison = EFactionTestMode::IsSame) const;

	/** @return the faction of an actor. None if the actor doesn't implement FactionAgentInterface */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DefaultToSelf = "Source"))
	static FFaction GetActorFaction(const AActor* Source);

	/** Sets the faction of an actor. Won't apply if the actor doesn't implement FactionAgentInterface.
	 * @param Target actor that will receive the new faction
	 * @param Faction that will be set
	 */
	UFUNCTION(BlueprintCallable, Category = Factions, meta = (DefaultToSelf = "Target"))
	static void SetActorFaction(AActor* Target, FFaction Faction);

	/** @return true if One and Other have the same faction */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DefaultToSelf = "A"))
	static bool ShareFaction(const AActor* A, const AActor* B);

	/**
	 * Replace the information of an existing faction
	 * @param Faction to change
	 * @param Descriptor to assign
	 * @return true if the faction was found and modified
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	bool SetDescriptor(const FFaction Faction, const FFactionDescriptor& Descriptor);

	/**
	 * @return all currently registered factions
	 */
	UFUNCTION(BlueprintCallable, Category = Factions, meta = (DisplayName = "Get All Factions"))
	void GetAllFactions(TArray<FFaction>& OutFactions) const;


	/** BLUEPRINT ONLY API */

	/** @return true if two factions are the same */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DisplayName = "Equals", CompactNodeTitle = "=="))
	static bool BPEquals(const FFaction A, const FFaction B);

	/** @return true if two factions are not the same */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DisplayName = "Not Equal", CompactNodeTitle = "!="))
	static bool BPNotEqual(const FFaction A, const FFaction B);

	/** @return true if this faction is none */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DisplayName = "Is None", BlueprintAutocast))
	static bool BPIsNone(const FFaction Faction);

	/**
	 * Find the information of a faction
	 * @param Faction to search for
	 * @param Descriptor of the faction, if found
	 * @return true if the faction was valid and information was found
	 */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DisplayName = "Get Descriptor"))
	bool BPGetDescriptor(const FFaction Faction, FFactionDescriptor& Descriptor) const;


protected:
	virtual void BeginDestroy() override;

	void OnWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS);

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif
};


inline TEnumAsByte<ETeamAttitude::Type> UFactionsSubsystem::GetActorAttitude(
	const AActor* Source, const AActor* Target) const
{
	return GetAttitude(GetActorFaction(Source), GetActorFaction(Target));
}

inline bool UFactionsSubsystem::IsHostile(const FFaction Source, const FFaction Target) const
{
	return GetAttitude(Source, Target) == ETeamAttitude::Hostile;
}

inline bool UFactionsSubsystem::IsFriendly(const FFaction Source, const FFaction Target) const
{
	return GetAttitude(Source, Target) == ETeamAttitude::Friendly;
}

inline bool UFactionsSubsystem::IsNeutral(const FFaction Source, const FFaction Target) const
{
	return GetAttitude(Source, Target) == ETeamAttitude::Neutral;
}

inline bool UFactionsSubsystem::IsActorHostile(const AActor* Source, const AActor* Target)
{
	return GetActorAttitude(Source, Target) == ETeamAttitude::Hostile;
}

inline bool UFactionsSubsystem::IsActorFriendly(const AActor* Source, const AActor* Target)
{
	return GetActorAttitude(Source, Target) == ETeamAttitude::Friendly;
}

inline bool UFactionsSubsystem::IsActorNeutral(const AActor* Source, const AActor* Target)
{
	return GetActorAttitude(Source, Target) == ETeamAttitude::Neutral;
}

inline FFaction UFactionsSubsystem::GetActorFaction(const AActor* Source)
{
	return IFactionAgentInterface::GetFaction(Source);
}

inline void UFactionsSubsystem::SetActorFaction(AActor* Target, FFaction Faction)
{
	IFactionAgentInterface::SetFaction(Target, Faction);
}

inline bool UFactionsSubsystem::ShareFaction(const AActor* A, const AActor* B)
{
	return GetActorFaction(A) == GetActorFaction(B);
}

inline bool UFactionsSubsystem::BPEquals(const FFaction A, const FFaction B)
{
	return A == B;
}

inline bool UFactionsSubsystem::BPNotEqual(const FFaction A, const FFaction B)
{
	return A != B;
}

inline bool UFactionsSubsystem::BPIsNone(const FFaction Faction)
{
	return Faction.IsNone();
}

inline UFactionsSubsystem* UFactionsSubsystem::Get(const UObject* ContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::ReturnNull))
	{
		return UGameInstance::GetSubsystem<UFactionsSubsystem>(World->GetGameInstance());
	}
	return nullptr;
}
