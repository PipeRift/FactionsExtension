// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "FactionAgentInterface.h"


//----------------------------------------------------------------------//
// UGenericTeamAgentInterface
//----------------------------------------------------------------------//
UFactionAgentInterface::UFactionAgentInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

const ETeamAttitude::Type IFactionAgentInterface::GetAttitude(const UObject& Target) const
{
	const auto* Factions = UFactionsSubsystem::Get(&Target);
	return Factions->GetAttitude(GetFaction(), GetFaction(&Target));
}

FFaction IFactionAgentInterface::GetFaction(const UObject* Source)
{
	if (Source && Source->Implements<UFactionAgentInterface>())
	{
		FFaction Faction;
		IFactionAgentInterface::Execute_EventGetFaction(Source, Faction);
		return Faction;
	}
	return FFaction::NoFaction;
}

void IFactionAgentInterface::SetFaction(UObject* Source, const FFaction& Faction)
{
	if (Source && Source->Implements<UFactionAgentInterface>())
	{
		IFactionAgentInterface::Execute_EventSetFaction(Source, Faction);
	}
}

FFaction IFactionAgentInterface::GetFaction() const
{
	return FFaction::NoFaction;
}

void IFactionAgentInterface::SetFaction(const FFaction& Faction)
{
	// By default call BP event
	IFactionAgentInterface::Execute_EventSetFaction(Cast<UObject>(this), Faction);
}
