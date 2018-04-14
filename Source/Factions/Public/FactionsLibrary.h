// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "Faction.h"

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

    UFUNCTION(BlueprintPure, Category = "Factions", meta = (CompactNodeTitle = "=="))
    static FORCEINLINE bool Equals(const FFaction& A, const FFaction& B) {
        return A == B;
    }

    UFUNCTION(BlueprintPure, Category = "Factions", meta = (CompactNodeTitle = "!="))
    static FORCEINLINE bool NotEqual(const FFaction& A, const FFaction& B) {
        return A != B;
    }

	/** Get the faction of an actor. None if the actors doesn't implement FactionAgentInterface */
	UFUNCTION(BlueprintPure, Category = "Factions")
	static FORCEINLINE FFaction GetFaction(AActor* Target)
	{
		return IFactionAgentInterface::Execute_GetFaction(Target);
	}

	/** @return true if this faction is none */
	UFUNCTION(BlueprintPure, Category = "Factions", meta = (DisplayName = "Is None"))
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
	UFUNCTION(BlueprintPure, Category = "Factions")
	static FORCEINLINE bool GetFactionInfo(const FFaction& Faction, FFactionInfo& Info)
	{
		return Faction.GetFactionInfo(Info);
	}

    UFUNCTION(BlueprintPure, Category = "Factions", meta = (DisplayName = "Get Attitude Towards", WorldContext = "A"))
	static FORCEINLINE TEnumAsByte<ETeamAttitude::Type> GetAttitudeBetween(AActor* A, AActor* B)
	{
		return GetAttitudeTowards(GetFaction(A), GetFaction(B));
	}

    UFUNCTION(BlueprintPure, Category = "Factions")
    static FORCEINLINE TEnumAsByte<ETeamAttitude::Type> GetAttitudeTowards(const FFaction& A, const FFaction& B)
	{
        return A.GetAttitudeTowards(B);
    }
};
