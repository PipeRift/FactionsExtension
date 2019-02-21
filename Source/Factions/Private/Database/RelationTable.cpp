// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "RelationTable.h"


void FRelationTable::IndexRelations()
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
