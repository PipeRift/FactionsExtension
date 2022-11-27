// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include "FactionTable.h"
#include "RelationTable.h"

#include <Engine/DataAsset.h>

#include "FactionCollection.generated.h"


#define LOCTEXT_NAMESPACE "FactionCollection"

/**
 * Struct containing information about a faction. Static use.
 */
UCLASS(Blueprintable)
class FACTIONS_API UFactionCollection : public UPrimaryDataAsset
{
	GENERATED_BODY()

protected:

	// Preffix to be added to faction ids where final ids become: {Scope}.{Id}
	UPROPERTY(EditAnywhere, Category = Collection)
	FString Scope;

	UPROPERTY(EditAnywhere, Category = Collection)
	FFactionTable Factions;

	UPROPERTY(EditAnywhere, Category = Collection)
	FRelationTable Relations;
};

#undef LOCTEXT_NAMESPACE
