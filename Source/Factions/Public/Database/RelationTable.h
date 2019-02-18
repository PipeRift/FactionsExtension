// Copyright 2015-2019 Piperift. All Rights Reserved.

#pragma once

#include "Faction.h"
#include "FactionRelation.h"
#include "RelationTable.generated.h"


#define LOCTEXT_NAMESPACE "RelationTable"

/**
 * Contains all faction relations
 */
USTRUCT()
struct FRelationTable
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, config, Category = Relations)
	TArray<FFactionRelation> ConfigList;

protected:

	UPROPERTY(Transient, SaveGame)
	TSet<FFactionRelation> IndexedList;


public:

	const TSet<FFactionRelation>& GetRelations() const { return IndexedList; }
	TSet<FFactionRelation>& GetRelations() { return IndexedList; }

	// Copies relations to a runtime set for fast searching
	void IndexRelations();
};


#undef LOCTEXT_NAMESPACE
