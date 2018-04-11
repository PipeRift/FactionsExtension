// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "Faction.h"
#include "FactionsSettings.h"
#include "FactionInfo.h"


const FFaction FFaction::NoFaction(NO_FACTION);

FFactionInfo* FFaction::GetFactionInfo() const
{
    UFactionsSettings* Settings = GetMutableDefault<UFactionsSettings>();

    if (Settings->Factions.IsValidIndex(Id)) {
        return &Settings->Factions[Id];
    }

    //If the faction is not found, return default faction info.
    return nullptr;
}

const ETeamAttitude::Type FFaction::GetAttitudeTowards(const FFaction& Other) const {
    const UFactionsSettings* Settings = GetDefault<UFactionsSettings>();

    const FFactionRelation* FoundRelationPtr = Settings->Relations.FindByKey(FFactionRelation(*this, Other));
    if (FoundRelationPtr == NULL) {
        const FFactionInfo* Info = GetFactionInfo();
        if (Info)
        {
            return Info->DefaultAttitude;
        }
        return ETeamAttitude::Neutral;
    }

    return FoundRelationPtr->Attitude;
}
