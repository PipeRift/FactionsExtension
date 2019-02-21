// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "FactionAgentInterface.h"



//----------------------------------------------------------------------//
// UGenericTeamAgentInterface
//----------------------------------------------------------------------//
UFactionAgentInterface::UFactionAgentInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

const ETeamAttitude::Type IFactionAgentInterface::GetAttitudeTowards(const AActor& Other) const
{
	const FFaction OtherFaction = IFactionAgentInterface::GetFaction(&Other);
	return GetFaction().GetAttitudeTowards(OtherFaction);
}

FFaction IFactionAgentInterface::GetFaction(const AActor* Other)
{
	if (Other && Other->Implements<UFactionAgentInterface>())
	{
		FFaction OtherFaction;
		IFactionAgentInterface::Execute_EventGetFaction(Other, OtherFaction);
		return OtherFaction;
	}
	return FFaction::NoFaction;
}

void IFactionAgentInterface::SetFaction(AActor* Other, const FFaction& NewFaction)
{
	if (Other && Other->Implements<UFactionAgentInterface>())
	{
		IFactionAgentInterface::Execute_EventSetFaction(Other, NewFaction);
	}
}

FFaction IFactionAgentInterface::GetFaction() const
{
	return FFaction::NoFaction;
}

void IFactionAgentInterface::SetFaction(const FFaction & Faction)
{
	// By default call BP event
	IFactionAgentInterface::Execute_EventSetFaction(Cast<UObject>(this), Faction);
}
