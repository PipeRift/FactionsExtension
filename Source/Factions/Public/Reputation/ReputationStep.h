// Copyright 2015-2019 Piperift. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "ReputationStep.generated.h"


USTRUCT(BlueprintType)
struct FReputationStep
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Step")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Step")
	int32 Length;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Step")
	FLinearColor Color;
};


