// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "Factions/Faction.h"
#include "Factions/FactionInfo.h"
#include "Factions/FactionAgentInterface.h"
#include "FactionsSettings.h"

#include "FactionsLibrary.generated.h"

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
		return IFactionAgentInterface::Execute_GetFaction(Target);
	}

	/** Sets the faction of an actor. Won't apply if the actor doesn't implement FactionAgentInterface.
	 * @param Target actor that will receive the new faction
	 * @param newFaction that will be set
	 */
	UFUNCTION(BlueprintCallable, Category = Factions, meta = (DefaultToSelf = "Target"))
	static void SetFaction(AActor* Target, const FFaction NewFaction)
	{
		IFactionAgentInterface::Execute_SetFaction(Target, NewFaction);
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

	/** @return true if A is Hostile to B (or otherwise) */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DefaultToSelf = "One"))
	static FORCEINLINE bool IsHostile(const AActor* One, const AActor* Other) {
		return GetAttitudeTowards(One, Other) == ETeamAttitude::Hostile;
	}

	/** @return true if A is Friendly to B (or otherwise) */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DefaultToSelf = "One"))
	static FORCEINLINE bool IsFriendly(const AActor* One, const AActor* Other) {
		return GetAttitudeTowards(One, Other) == ETeamAttitude::Friendly;
	}

	/** @return true if A is Neutral to B (or otherwise) */
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
	 * Registry a new faction.
	 * This will not check if another faction of the same name exists. Use FindFactionByName for that. 
	 * @param FactionInfo that the new faction will have. For things like name, color or default attitudes.
	 * @return the created faction
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	static FFaction RegistryFaction(const FName& Name, const FFactionInfo& FactionInfo);

	/**
	* Removes a faction from the database
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
};
