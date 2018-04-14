// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "Faction.h"
#include "FactionsSettings.h"
#include "FactionInfo.h"


const FFaction FFaction::NoFaction(NO_FACTION);

bool FFaction::GetFactionInfo(FFactionInfo& Info) const
{
	const UFactionsSettings* Settings = GetDefault<UFactionsSettings>();
	check(Settings);

	if (Settings->Factions.IsValidIndex(Id))
    {
		Info = Settings->Factions[Id];
		return true;
	}

	//If the faction is not found, return default faction info.
	return false;
}

const ETeamAttitude::Type FFaction::GetAttitudeTowards(const FFaction& Other) const
{
    const UFactionsSettings* Settings = GetDefault<UFactionsSettings>();

    const FFactionRelation* FoundRelationPtr = Settings->Relations.FindByKey(FFactionRelation(*this, Other));
    if (FoundRelationPtr == NULL)
    {
		//Relation not found, use default
		FFactionInfo Info;
        if (GetFactionInfo(Info))
        {
			if (*this == Other)
			{
				return Info.AttitudeTowardsItself;
			}
			return Info.DefaultAttitudeTowardsOthers;
        }
        return ETeamAttitude::Neutral;
    }

    return FoundRelationPtr->Attitude;
}
