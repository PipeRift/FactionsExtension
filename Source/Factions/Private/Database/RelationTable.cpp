// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "RelationTable.h"


void FRelationTable::RefreshIndexCache()
{
	// Avoid memory deallocation
	IndexedList.Empty(ConfigList.Num());
	// Reduce possible memory allocation
	IndexedList.Reserve(ConfigList.Num());

	for (const auto& Relation : ConfigList)
	{
		IndexedList.Add(Relation);
	}
}
