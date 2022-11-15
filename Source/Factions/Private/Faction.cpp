// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "Faction.h"

#include "FactionBehavior.h"
#include "FactionDescriptor.h"
#include "FactionsModule.h"
#include "FactionsSubsystem.h"


const FFaction FFaction::NoFaction(NO_FACTION_NAME);

FFaction::FFaction(const FGenericTeamId& InTeam)
{
	if (InTeam.GetId() != FGenericTeamId::NoTeam.GetId())
	{
		// TODO: Shouldn't access subsystem using GEngine
		const UFactionsSubsystem* Settings = UFactionsSubsystem::Get(GEngine);
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

const FFactionDescriptor* FFaction::GetDescriptor() const
{
	// TODO: Shouldn't access subsystem using GEngine
	const UFactionsSubsystem* Settings = UFactionsSubsystem::Get(GEngine);
	check(Settings);

	return Settings->GetFactions().GetDescriptor(*this);
}

bool FFaction::SetDescriptor(const FFactionDescriptor& Descriptor) const
{
	// TODO: Shouldn't access subsystem using GEngine
	UFactionsSubsystem* Settings = UFactionsSubsystem::Get(GEngine);
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

	// TODO: Shouldn't access subsystem using GEngine
	const UFactionsSubsystem* Settings = UFactionsSubsystem::Get(GEngine);
	check(Settings);
	return !Settings->GetFactions().Descriptors.Contains(Name);
}

const ETeamAttitude::Type FFaction::GetAttitudeTowards(const FFaction& Other) const
{
	// TODO: Shouldn't access subsystem using GEngine
	const UFactionsSubsystem* Settings = UFactionsSubsystem::Get(GEngine);

	const FFactionRelation* FoundRelationPtr = Settings->FindRelation(*this, Other);
	if (FoundRelationPtr == nullptr)
	{
		// Relation not found, use default
		if (const auto* Behavior = GetDescriptor())
		{
			return *this == Other ? Behavior->SelfAttitude : Behavior->ExternalAttitude;
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
	if (IsNone())
	{
		return FGenericTeamId::NoTeam;
	}

	// TODO: Shouldn't access subsystem using GEngine
	const UFactionsSubsystem* Settings = UFactionsSubsystem::Get(GEngine);
	check(Settings);

	TArray<FName> Keys;
	Settings->GetFactions().Descriptors.GetKeys(Keys);

	// Find Id
	const int32 Id = Keys.IndexOfByKey(Name);
	if (Id == INDEX_NONE || Id >= FGenericTeamId::NoTeam.GetId())
	{
		// If Faction ID is 255 or higher, Teams won't support it.
		return FGenericTeamId::NoTeam;
	}

	return {static_cast<uint8>(Id)};
}
