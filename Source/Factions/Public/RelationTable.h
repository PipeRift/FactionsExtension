// Copyright 2015-2020 Piperift. All Rights Reserved.

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
	TSet<FFactionRelation> List;

public:

	int32 Num() const { return List.Num(); }
};


#undef LOCTEXT_NAMESPACE
