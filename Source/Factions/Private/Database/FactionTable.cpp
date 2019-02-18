// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "FactionTable.h"


FFaction FFactionTable::RegistryFaction(FName Name, FFactionInfo Info)
{
	if (Items.Contains(Name))
		return {};

	Items.Add(Name, Info);
	return { Name };
}

bool FFactionTable::UnregistryFaction(FFaction Faction)
{
	if (Faction.IsNone())
		return false;

	return Items.Remove(Faction.GetIdName()) > 0;
}

bool FFactionTable::SetInfo(FFaction Faction, const FFactionInfo& NewInfo)
{
	if (Faction.IsNone())
		return false;

	FFactionInfo* const Info = Items.Find(Faction.GetIdName());
	if (Info)
	{
		*Info = NewInfo;
		return true;
	}
	return false;
}
