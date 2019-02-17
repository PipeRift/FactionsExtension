// Copyright 2015-2019 Piperift. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "Factions/Faction.h"
#include "Database/FactionInfo.h"
#include "Factions/FactionAgentInterface.h"
#include "FactionsSettings.h"

#include "FactionsLibrary.generated.h"


UENUM()
enum class EFactionTestMode : uint8
{
	IsTheSame,
	IsFriendly,
	IsNeutral,
	IsHostile
};


/**
 *
 */
UCLASS()
class FACTIONS_API UFactionsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/***************************************/
	/* Factions                            */
	/***************************************/

	/** @return true if two factions are the same */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (CompactNodeTitle = "=="))
	static FORCEINLINE bool Equals(const FFaction A, const FFaction B) {
		return A == B;
	}

	/** @return true if two factions are not the same */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (CompactNodeTitle = "!="))
	static FORCEINLINE bool NotEqual(const FFaction A, const FFaction B) {
		return A != B;
	}

	/** @return the faction of an actor. None if the actor doesn't implement FactionAgentInterface */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DefaultToSelf = "Target"))
	static FORCEINLINE FFaction GetFaction(const AActor* Target)
	{
		return IFactionAgentInterface::GetFaction(Target);
	}

	/** Sets the faction of an actor. Won't apply if the actor doesn't implement FactionAgentInterface.
	 * @param Target actor that will receive the new faction
	 * @param newFaction that will be set
	 */
	UFUNCTION(BlueprintCallable, Category = Factions, meta = (DefaultToSelf = "Target"))
	static void SetFaction(AActor* Target, const FFaction NewFaction)
	{
		IFactionAgentInterface::SetFaction(Target, NewFaction);
	}

	/** @return true if this faction is none */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DisplayName = "Is None", BlueprintAutocast))
	static FORCEINLINE bool FactionIsNone(const FFaction Faction)
	{
		return Faction.IsNone();
	}

	/**
	 * Find the information of a faction
	 * @param Faction to search for
	 * @param Info associated to the faction, if found
	 * @return true if the faction was valid and information was found
	 */
	UFUNCTION(BlueprintPure, Category = Factions)
	static FORCEINLINE bool GetFactionInfo(const FFaction Faction, FFactionInfo& Info)
	{
		return Faction.GetFactionInfo(Info);
	}

	/**
	 * Replace the information of an existing faction
	 * @param Faction to change
	 * @param Info to replace the previous one
	 * @return true if the faction was found and modified
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	static bool SetFactionInfo(const FFaction Faction, const FFactionInfo& Info) {
		return Faction.SetFactionInfo(Info);
	}

	/** @return One's attitude towards Other actor */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DefaultToSelf = "One"))
	static FORCEINLINE TEnumAsByte<ETeamAttitude::Type> GetAttitudeTowards(const AActor* One, const AActor* Other)
	{
		return GetAttitudeToFaction(GetFaction(One), GetFaction(Other));
	}

	/** @return true if One and Other have the same faction */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DefaultToSelf = "One"))
	static FORCEINLINE bool SharesFaction(const AActor* One, const AActor* Other) {
		return GetFaction(One) == GetFaction(Other);
	}

	/** @return true if One is Hostile to Other (or otherwise) */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DefaultToSelf = "One"))
	static FORCEINLINE bool IsHostile(const AActor* One, const AActor* Other) {
		return GetAttitudeTowards(One, Other) == ETeamAttitude::Hostile;
	}

	/** @return true if One is Friendly to Other (or otherwise) */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DefaultToSelf = "One"))
	static FORCEINLINE bool IsFriendly(const AActor* One, const AActor* Other) {
		return GetAttitudeTowards(One, Other) == ETeamAttitude::Friendly;
	}

	/** @return true if One is Neutral to Other (or otherwise) */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DefaultToSelf = "One"))
	static FORCEINLINE bool IsNeutral(const AActor* One, const AActor* Other) {
		return GetAttitudeTowards(One, Other) == ETeamAttitude::Neutral;
	}


	/** @return One's attitude towards Other faction */
	UFUNCTION(BlueprintPure, Category = Factions)
	static FORCEINLINE TEnumAsByte<ETeamAttitude::Type> GetAttitudeToFaction(const FFaction One, const FFaction Other)
	{
		return One.GetAttitudeTowards(Other);
	}

	/** @return true if A is Hostile to B (or otherwise) */
	UFUNCTION(BlueprintPure, Category = Factions)
	static FORCEINLINE bool IsHostileFaction(const FFaction One, const FFaction Other) {
		return GetAttitudeToFaction(One, Other) == ETeamAttitude::Hostile;
	}

	/** @return true if A is Friendly to B (or otherwise) */
	UFUNCTION(BlueprintPure, Category = Factions)
	static FORCEINLINE bool IsFriendlyFaction(const FFaction One, const FFaction Other) {
		return GetAttitudeToFaction(One, Other) == ETeamAttitude::Friendly;
	}

	/** @return true if One is Neutral to Other (or otherwise) */
	UFUNCTION(BlueprintPure, Category = Factions)
	static FORCEINLINE bool IsNeutralFaction(const FFaction One, const FFaction Other) {
		return GetAttitudeToFaction(One, Other) == ETeamAttitude::Neutral;
	}
	/**
	* Find the information of a faction
	* @param Faction to search for
	* @param Info associated to the faction, if found
	* @return true if the faction was valid and information was found
	*/
	UFUNCTION(BlueprintPure, Category = Factions)
	static FORCEINLINE FString GetDisplayName(const FFaction Faction)
	{
		return Faction.GetDisplayName();
	}

	/**
	 * Registry a new faction.
	 * This will not check if another faction of the same name exists. Use FindFactionByName for that.
	 * @param FactionInfo that the new faction will have. For things like name, color or default attitudes.
	 * @return the created faction
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	static FFaction RegistryFaction(const FName& Name, const FFactionInfo& FactionInfo);

	/**
	* Remove a faction from the database
	* @param Faction to remove.
	* @return true if the faction was removed
	*/
	UFUNCTION(BlueprintCallable, Category = Factions)
	static bool UnregistryFaction(FFaction Faction);

	/**
	 * @return all currently registered factions
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	static void GetAllFactions(TArray<FFaction>& Factions);


	/**
	* Registry a new relation between two factions into the database
	* @param NewRelation to be registered.
	* @return true if the relation was registered, false if the two factions were the same or invalid.
	*/
	UFUNCTION(BlueprintCallable, Category = Factions)
	static bool RegistryRelation(const FFactionRelation& NewRelation);

	/**
	* Remove a relation from the database
	* @param Relation to be unregistered
	* @return true if the relation was unregistered
	*/
	UFUNCTION(BlueprintCallable, Category = Factions)
	static bool UnregistryRelation(const FFactionRelation& Relation);

	/**
	 * Get all actors in the world with a certain faction
	 * @param Faction to check actors for
	 * @param Comparison relation between the actor and the faction
	 * @param ActorClass limits the actors to be found by that class
	 * @return true if operation was successful and the array of actors
	 */
	UFUNCTION(BlueprintCallable, Category = Factions, meta = (WorldContext = "ContextObject", AdvancedDisplay="ActorClass", DeterminesOutputType = "ActorClass", DynamicOutputParam = "OutActors"))
	static bool GetAllActorsWithFaction(const UObject* ContextObject, const FFaction Faction, EFactionTestMode Comparison, TSubclassOf<AActor> ActorClass, TArray<AActor*>& OutActors);
};
