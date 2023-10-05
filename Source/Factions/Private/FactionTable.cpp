// Copyright 2015-2023 Piperift. All Rights Reserved.

#include "FactionTable.h"


FFactionDescriptor* FFactionTable::GetDescriptor(FFaction Faction)
{
	return !Faction.IsNone() ? List.Find(Faction.GetId()) : nullptr;
}

const FFactionDescriptor* FFactionTable::GetDescriptor(FFaction Faction) const
{
	return !Faction.IsNone() ? List.Find(Faction.GetId()) : nullptr;
}
