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

	UPROPERTY(EditAnywhere, config)
	TArray<FFactionRelation> ConfigList;

	UPROPERTY(SaveGame)
	TSet<FFactionRelation> IndexedList;
};


#undef LOCTEXT_NAMESPACE
