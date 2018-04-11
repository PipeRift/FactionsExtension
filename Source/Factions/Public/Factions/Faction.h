// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "GenericTeamAgentInterface.h"

#include "Faction.generated.h"

#define NO_FACTION -1
#define NO_FACTION_NAME FString("None")

struct FFactionInfo;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FACTIONS_API FFaction
{
    GENERATED_USTRUCT_BODY()

    FFaction() : Id(NO_FACTION) {}

    FFaction(int32 InId) : Id(InId) {}

    FFaction(const FGenericTeamId& InTeam)
    {
        if (InTeam.GetId() == FGenericTeamId::NoTeam.GetId())
            Id = NO_FACTION;
        else
            Id = InTeam.GetId();
    }

    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Id;


    FFactionInfo* GetFactionInfo() const;

    /*UFUNCTION(BlueprintPure, Category = Faction, meta = (DisplayName = "Get Faction Info"))
    FORCEINLINE const FFactionInfo& exec_GetFactionInfo() const {
        return *GetFactionInfo();
    }*/

    FORCEINLINE const FGenericTeamId GetTeam() const {
        return Id != NO_FACTION? FGenericTeamId(Id) : FGenericTeamId::NoTeam;
    }

    FORCEINLINE bool IsNone() const {
        return Id == NO_FACTION;
    }

    /**
     * Attitude evaluation
     */
    FORCEINLINE bool IsHostileTowards(const FFaction& Other) const {
        return GetAttitudeTowards(Other) == ETeamAttitude::Hostile;
    }

    const ETeamAttitude::Type GetAttitudeTowards(const FFaction& Other) const;

    /**
     * Operator overloading & Hashes
     */
    FORCEINLINE bool operator==(const FFaction& Other) const { return Id == Other.Id; }
    FORCEINLINE bool operator!=(const FFaction& Other) const { return !(*this == Other); }

    // Implicit conversion to GenericTeamId
    operator FGenericTeamId() const
    {
        return GetTeam();
    }

    friend uint32 GetTypeHash(const FFaction& InRelation)
    {
        return InRelation.IsNone() ? MAX_uint32 : (uint32)InRelation.Id;
    }

    static const FFaction NoFaction;
};
