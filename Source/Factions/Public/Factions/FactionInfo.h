// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "GenericTeamAgentInterface.h"

#include "Faction.h"
#include "FactionInfo.generated.h"

class UFactionsSettings;

/**
 * Defines a relation between factions
*/
USTRUCT(BlueprintType)
struct FACTIONS_API FFactionRelation
{
    GENERATED_USTRUCT_BODY()
    
    FFactionRelation() : 
        FactionA(FFaction::NoFaction), FactionB(FFaction::NoFaction), Attitude(ETeamAttitude::Neutral)
    {}

    FFactionRelation(FFaction A, FFaction B, ETeamAttitude::Type InAttitude = ETeamAttitude::Neutral) :
        FactionA(A), FactionB(B), Attitude(InAttitude)
    {}


    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "A"))
    FFaction FactionA;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "B"))
    FFaction FactionB;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<ETeamAttitude::Type> Attitude;


    bool IsValid() const
    {
        return !FactionA.IsNone() && !FactionB.IsNone();
    }

    FORCEINLINE bool operator==(const FFactionRelation& Other) const {
        return (FactionA == Other.FactionA && FactionB == Other.FactionB) ||
               (FactionA == Other.FactionB && FactionB == Other.FactionA);
    }
    FORCEINLINE bool operator!=(const FFactionRelation& Other) const { return !(*this == Other); }
    
    friend uint32 GetTypeHash(const FFactionRelation& InRelation)
    {
        return GetTypeHash(InRelation.FactionA) ^ GetTypeHash(InRelation.FactionB);
        /*return GetTypeHash(InRelation.Guid);*/
    }
};

/**
 * Struct containing information about a faction. Static use.
 */
USTRUCT(BlueprintType)
struct FACTIONS_API FFactionInfo
{
    GENERATED_USTRUCT_BODY()

public:
    FFactionInfo() : Name(NO_FACTION_NAME), Color(FColor::Cyan), DefaultAttitude(ETeamAttitude::Neutral)
    {}

    FFactionInfo(FString InName, FColor InColor = FColor::Cyan) :
        Name(InName),
        Color(InColor),
        DefaultAttitude(ETeamAttitude::Neutral) 
    {}


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Faction)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<ETeamAttitude::Type> DefaultAttitude;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Faction)
    FColor Color;

    const FFaction GetFaction();

    void SetRelation(const FFaction& OtherFaction, const ETeamAttitude::Type Attitude);
};
