// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "FactionTable.h"


FFaction FFactionTable::AddFaction(FName Id, const FFactionBehavior& Behavior,
								   const FFactionDescriptor& Descriptor)
{
	Behaviors.Emplace(Id, Behavior);
	Descriptors.Emplace(Id, Descriptor);
	return {Id};
}

void FFactionTable::RemoveFaction(FFaction Faction)
{
	if (!Faction.IsNone())
	{
		Behaviors.Remove(Faction.GetIdName());
		Descriptors.Remove(Faction.GetIdName());
	}
}

FFactionBehavior* FFactionTable::GetBehavior(FFaction Faction)
{
	return (!Faction.IsNone())? Behaviors.Find(Faction.GetIdName()) : nullptr;
}

const FFactionBehavior* FFactionTable::GetBehavior(FFaction Faction) const
{
	return (!Faction.IsNone())? Behaviors.Find(Faction.GetIdName()) : nullptr;
}

FFactionDescriptor* FFactionTable::GetDescriptor(FFaction Faction)
{
	return (!Faction.IsNone())? Descriptors.Find(Faction.GetIdName()) : nullptr;
}

const FFactionDescriptor* FFactionTable::GetDescriptor(FFaction Faction) const
{
	return (!Faction.IsNone())? Descriptors.Find(Faction.GetIdName()) : nullptr;
}
