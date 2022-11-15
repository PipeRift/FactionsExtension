// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "FactionTable.h"


FFaction FFactionTable::AddFaction(FName Id, const FFactionDescriptor& Descriptor)
{
	Descriptors.Emplace(Id, Descriptor);
	return {Id};
}

void FFactionTable::RemoveFaction(FFaction Faction)
{
	if (!Faction.IsNone())
	{
		Descriptors.Remove(Faction.GetIdName());
	}
}

FFactionDescriptor* FFactionTable::GetDescriptor(FFaction Faction)
{
	return (!Faction.IsNone())? Descriptors.Find(Faction.GetIdName()) : nullptr;
}

const FFactionDescriptor* FFactionTable::GetDescriptor(FFaction Faction) const
{
	return (!Faction.IsNone())? Descriptors.Find(Faction.GetIdName()) : nullptr;
}
