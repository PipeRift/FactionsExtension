// Copyright 2016-2017 Frontwire Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvQueryTest_AI.h"
#include "EnvQueryTest_IsCurrentTarget.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Is Current Target"))
class AIEXTENSION_API UEnvQueryTest_IsCurrentTarget : public UEnvQueryTest_AI
{
    GENERATED_UCLASS_BODY()

protected:
    virtual void RunAITest(AAIGeneric* AI, FEnvQueryInstance& QueryInstance) const;
    virtual FText GetDescriptionDetails() const override;
};
