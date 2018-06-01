// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "RelationDatabase.h"


void FRelationDatabase::IndexRelations()
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
