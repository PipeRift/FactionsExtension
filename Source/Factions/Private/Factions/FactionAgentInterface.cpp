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
	const FFaction OtherFaction = IFactionAgentInterface::Execute_GetFaction(&Other);
	return GetFaction().GetAttitudeTowards(OtherFaction);
}

const FFaction IFactionAgentInterface::Execute_GetFaction(const AActor* Other)
{
	if (Other && Other->Implements<UFactionAgentInterface>())
	{
		const IFactionAgentInterface* OtherFactionAgent = Cast<IFactionAgentInterface>(Other);
		if (OtherFactionAgent)
		{
			//C++ Interface
			return OtherFactionAgent->GetFaction();
		}
		else
		{
			//BP Interface
			FFaction OtherFaction;
			IFactionAgentInterface::Execute_EventGetFaction(Other, OtherFaction);
			return OtherFaction;
		}
	}
	return FFaction::NoFaction;
}

void IFactionAgentInterface::Execute_SetFaction(AActor* Other, const FFaction& NewFaction)
{
	if (Other && Other->Implements<UFactionAgentInterface>())
	{
		IFactionAgentInterface* OtherFactionAgent = Cast<IFactionAgentInterface>(Other);
		if (OtherFactionAgent)
		{
			//C++ Interface
			OtherFactionAgent->SetFaction(NewFaction);
		}
		else
		{
			//BP Interface
			IFactionAgentInterface::Execute_EventSetFaction(Other, NewFaction);
		}
	}
}

FFaction IFactionAgentInterface::GetFaction() const
{
	FFaction Faction;
	// By default call BP event
	IFactionAgentInterface::Execute_EventGetFaction(Cast<UObject>(this), Faction);
	return Faction;
}

void IFactionAgentInterface::SetFaction(const FFaction & Faction)
{
	// By default call BP event
	IFactionAgentInterface::Execute_EventSetFaction(Cast<UObject>(this), Faction);
}
