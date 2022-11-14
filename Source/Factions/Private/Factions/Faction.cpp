// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "Faction.h"
#include "FactionsSettings.h"
#include "FactionDescriptor.h"
#include "FactionBehavior.h"
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
			Settings->GetFactions().Descriptors.GetKeys(Keys);

			if (Keys.IsValidIndex(InTeam.GetId()))
			{
				Name = Keys[InTeam.GetId()];
				return;
			}
		}
	}
	Name = NO_FACTION_NAME;
}

const FFactionBehavior* FFaction::GetBehavior() const
{
	UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	return Settings->GetFactions().GetBehavior(*this);
}

bool FFaction::SetBehavior(const FFactionBehavior& Behavior) const
{
	UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	if (auto* FoundBehavior = Settings->GetFactions().GetBehavior(*this))
	{
		*FoundBehavior = Behavior;
		Settings->MarkPackageDirty();
		return true;
	}
	return false;
}

const FFactionDescriptor* FFaction::GetDescriptor() const
{
	UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	return Settings->GetFactions().GetDescriptor(*this);
}

bool FFaction::SetDescriptor(const FFactionDescriptor& Descriptor) const
{
	UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	if (auto* FoundDescriptor = Settings->GetFactions().GetDescriptor(*this))
	{
		*FoundDescriptor = Descriptor;
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
	return !Settings->GetFactions().Descriptors.Contains(Name);
}

const ETeamAttitude::Type FFaction::GetAttitudeTowards(const FFaction& Other) const
{
	const UFactionsSettings* Settings = FFactionsModule::GetFactionManager();

	const FFactionRelation* FoundRelationPtr = Settings->FindRelation(*this, Other);
	if (FoundRelationPtr == nullptr)
	{
		//Relation not found, use default
		if (const auto* Behavior = GetBehavior())
		{
			return *this == Other? Behavior->SelfAttitude : Behavior->DefaultAttitude;
		}
		return ETeamAttitude::Neutral;
	}

	return FoundRelationPtr->Attitude;
}

FString FFaction::GetDisplayName() const
{
	if (const auto* Descriptor = GetDescriptor())
	{
		return Descriptor->DisplayName.IsEmpty() ? ToString() : Descriptor->DisplayName.ToString();
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
	Settings->GetFactions().Descriptors.GetKeys(Keys);

	//Find Id
	const int32 Id = Keys.IndexOfByKey(Name);
	if (Id == INDEX_NONE || Id >= FGenericTeamId::NoTeam.GetId())
	{
		// If Faction ID is 255 or higher, Teams won't support it.
		return FGenericTeamId::NoTeam;
	}

	return { static_cast<uint8>(Id) };
}
