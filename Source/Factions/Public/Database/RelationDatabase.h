// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "GenericTeamAgentInterface.h"

#include "Faction.h"
#include "FactionRelation.h"
#include "RelationDatabase.generated.h"

#define LOCTEXT_NAMESPACE "RelationDatabase"

/**
 * Contains all faction relations
 */
USTRUCT()
struct FRelationDatabase
{
	GENERATED_USTRUCT_BODY()

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
