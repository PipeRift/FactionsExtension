// Copyright 2016-2017 Frontwire Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_Faction.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Faction"))
class FACTIONS_API UEnvQueryTest_Faction : public UEnvQueryTest
{
    GENERATED_UCLASS_BODY()

protected:
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
    virtual FText GetDescriptionDetails() const override;
};
