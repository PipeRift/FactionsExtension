// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "Factions/Faction.h"
#include "Factions/FactionInfo.h"
#include "Factions/FactionAgentInterface.h"

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

    UFUNCTION(BlueprintPure, Category = Factions, meta = (CompactNodeTitle = "=="))
    static FORCEINLINE bool Equals(const FFaction& A, const FFaction& B) {
        return A == B;
    }

    UFUNCTION(BlueprintPure, Category = Factions, meta = (CompactNodeTitle = "!="))
    static FORCEINLINE bool NotEqual(const FFaction& A, const FFaction& B) {
        return A != B;
    }

	/** Get the faction of an actor. None if the actors doesn't implement FactionAgentInterface */
	UFUNCTION(BlueprintPure, Category = Factions)
	static FORCEINLINE FFaction GetFaction(const AActor* Target)
	{
		return IFactionAgentInterface::Execute_GetFaction(Target);
	}

	/** @return true if this faction is none */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DisplayName = "Is None", BlueprintAutocast))
	static FORCEINLINE bool FactionIsNone(const FFaction& Faction)
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
	static FORCEINLINE bool GetFactionInfo(const FFaction& Faction, FFactionInfo& Info)
	{
		return Faction.GetFactionInfo(Info);
	}

	/** @return One's attitude towards Other actor */
    UFUNCTION(BlueprintPure, Category = Factions, meta = (WorldContext = "One"))
	static FORCEINLINE TEnumAsByte<ETeamAttitude::Type> GetAttitudeTowards(const AActor* One, const AActor* Other)
	{
		return GetAttitudeToFaction(GetFaction(One), GetFaction(Other));
	}

	/** @return true if A is Hostile to B (or otherwise) */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (WorldContext = "One"))
	static FORCEINLINE bool IsHostile(const AActor* One, const AActor* Other) {
		return GetAttitudeTowards(One, Other) == ETeamAttitude::Hostile;
	}

	/** @return true if A is Friendly to B (or otherwise) */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (WorldContext = "One"))
	static FORCEINLINE bool IsFriendly(const AActor* One, const AActor* Other) {
		return GetAttitudeTowards(One, Other) == ETeamAttitude::Friendly;
	}

	/** @return true if A is Neutral to B (or otherwise) */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (WorldContext = "One"))
	static FORCEINLINE bool IsNeutral(const AActor* One, const AActor* Other) {
		return GetAttitudeTowards(One, Other) == ETeamAttitude::Neutral;
	}


	/** @return One's attitude towards Other faction */
	UFUNCTION(BlueprintPure, Category = Factions)
	static FORCEINLINE TEnumAsByte<ETeamAttitude::Type> GetAttitudeToFaction(const FFaction& One, const FFaction& Other)
	{
		return One.GetAttitudeTowards(Other);
	}

	/** @return true if A is Hostile to B (or otherwise) */
	UFUNCTION(BlueprintPure, Category = Factions)
	static FORCEINLINE bool IsHostileFaction(const FFaction& One, const FFaction& Other) {
		return GetAttitudeToFaction(One, Other) == ETeamAttitude::Hostile;
	}

	/** @return true if A is Friendly to B (or otherwise) */
	UFUNCTION(BlueprintPure, Category = Factions)
	static FORCEINLINE bool IsFriendlyFaction(const FFaction& One, const FFaction& Other) {
		return GetAttitudeToFaction(One, Other) == ETeamAttitude::Friendly;
	}

	/** @return true if One is Neutral to Other (or otherwise) */
	UFUNCTION(BlueprintPure, Category = Factions)
	static FORCEINLINE bool IsNeutralFaction(const FFaction& One, const FFaction& Other) {
		return GetAttitudeToFaction(One, Other) == ETeamAttitude::Neutral;
	}
};
