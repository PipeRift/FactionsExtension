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

	UFUNCTION(BlueprintPure, Category = "Factions")
	static FORCEINLINE FFaction GetFaction(AActor* Target)
	{
		return IFactionAgentInterface::Execute_GetFaction(Target);
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
