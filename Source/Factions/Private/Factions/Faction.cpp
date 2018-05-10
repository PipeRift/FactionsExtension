// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "Faction.h"
#include "FactionsSettings.h"
#include "FactionInfo.h"
#include "FactionsModule.h"


const FFaction FFaction::NoFaction(NO_FACTION_NAME);

bool FFaction::GetFactionInfo(FFactionInfo& Info) const
{
	const UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	const FFactionInfo* FoundInfo = Settings->Factions.Find(Name);
	if (FoundInfo)
    {
		Info = *FoundInfo;
		return true;
	}

	//If the faction is not found, return default faction info.
	return false;
}

bool FFaction::SetFactionInfo(const FFactionInfo& NewInfo) const
{
	UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	FFactionInfo* FoundInfo = Settings->Factions.Find(Name);
	if (FoundInfo)
	{
		*FoundInfo = NewInfo;
		Settings->MarkPackageDirty();
		return true;
	}

	// If the faction is not found, we couldn't have set the new Info
	return false;
}

bool FFaction::IsNone() const
{
	if (Name == NO_FACTION_NAME)
		return true;

	const UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);
	return !Settings->Factions.Contains(Name);
}

const ETeamAttitude::Type FFaction::GetAttitudeTowards(const FFaction& Other) const
{
    const UFactionsSettings* Settings = FFactionsModule::GetFactionManager();

	const FFactionRelation* FoundRelationPtr = Settings->Relations.Find({ *this, Other });
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
