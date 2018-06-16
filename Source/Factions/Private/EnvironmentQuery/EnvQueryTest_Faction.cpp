// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "EnvQueryTest_Faction.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"

#define LOCTEXT_NAMESPACE "UEnvQueryTest_Faction"


UEnvQueryTest_Faction::UEnvQueryTest_Faction(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    TestPurpose = EEnvTestPurpose::Filter;

    Cost = EEnvTestCost::Low;
    SetWorkOnFloatValues(false);
    ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();

}

void UEnvQueryTest_Faction::RunTest(FEnvQueryInstance& QueryInstance) const
{
}

FText UEnvQueryTest_Faction::GetDescriptionDetails() const
{
    return Super::GetDescriptionDetails();
}

#undef LOCTEXT_NAMESPACE
