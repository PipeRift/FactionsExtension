// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "Faction.h"
#include "FactionsSettings.h"
#include "FactionInfo.h"
#include "FactionsModule.h"


const FFaction FFaction::NoFaction(NO_FACTION_NAME);

FFaction::FFaction(const FGenericTeamId& InTeam)
{
	if (InTeam.GetId() != FGenericTeamId::NoTeam.GetId())
	{
		const UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
		if (Settings)
		{
			TArray<FName> Keys;
			Settings->GetFactionInfos().GetKeys(Keys);

			if (Keys.IsValidIndex(InTeam.GetId()))
			{
				Name = Keys[InTeam.GetId()];
				return;
			}
		}
	}
	Name = NO_FACTION_NAME;
}

bool FFaction::GetFactionInfo(FFactionInfo& Info) const
{
	const UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	const FFactionInfo* FoundInfo = Settings->GetFactionInfos().Find(Name);
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

	if (Settings->GetFactionTable().SetInfo(*this, NewInfo))
	{
		Settings->MarkPackageDirty();
		return true;
	}
	return false;
}

bool FFaction::IsNone() const
{
	if (Name == NO_FACTION_NAME)
		return true;

	const UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);
	return !Settings->GetFactionInfos().Contains(Name);
}

const ETeamAttitude::Type FFaction::GetAttitudeTowards(const FFaction& Other) const
{
	const UFactionsSettings* Settings = FFactionsModule::GetFactionManager();

	const FFactionRelation* FoundRelationPtr = Settings->FindRelation(*this, Other);
	if (FoundRelationPtr == nullptr)
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

FString FFaction::GetDisplayName() const
{
	FFactionInfo Info;
	if (GetFactionInfo(Info))
	{
		return Info.DisplayName.IsEmpty() ? ToString() : Info.DisplayName.ToString();
	}
	return ToString();
}

const FGenericTeamId FFaction::GetTeam() const
{
	if (IsNone()) {
		return FGenericTeamId::NoTeam;
	}

	const UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	TArray<FName> Keys;
	Settings->GetFactionInfos().GetKeys(Keys);

	//Find Id
	const int32 Id = Keys.IndexOfByKey(Name);
	if (Id == INDEX_NONE || Id >= FGenericTeamId::NoTeam.GetId())
	{
		// If Faction ID is 255 or higher, Teams won't support it.
		return FGenericTeamId::NoTeam;
	}

	return { static_cast<uint8>(Id) };
}
