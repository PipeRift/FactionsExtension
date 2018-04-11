// Copyright 2016-2017 Frontwire Studios. All Rights Reserved.

#include "AIExtensionPrivatePCH.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"
#include "EnvQueryTest_IsCurrentTarget.h"


#define LOCTEXT_NAMESPACE "UEnvQueryTest_IsCurrentTarget"

UEnvQueryTest_IsCurrentTarget::UEnvQueryTest_IsCurrentTarget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    TestPurpose = EEnvTestPurpose::Filter;

    Cost = EEnvTestCost::Low;
    SetWorkOnFloatValues(false);
    ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();

    BoolValue.DefaultValue = false;
}

void UEnvQueryTest_IsCurrentTarget::RunAITest(AAIGeneric* AI, FEnvQueryInstance& QueryInstance) const
{
    UObject* Owner = QueryInstance.Owner.Get();
    if (!Owner)
        return;

    BoolValue.BindData(Owner, QueryInstance.QueryID);
    const bool bNegate = BoolValue.GetValue();

    const APawn* CurrentTarget = AI->GetTarget();

    for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
    {
        const AActor* ItemActor = GetItemActor(QueryInstance, It.GetIndex());
        const bool bIsSameTarget = CurrentTarget == ItemActor;

        It.SetScore(TestPurpose, FilterType, bIsSameTarget, !bNegate);
    }
}

FText UEnvQueryTest_IsCurrentTarget::GetDescriptionDetails() const
{
    if (BoolValue.IsDynamic())
        return Super::GetDescriptionDetails();

    return BoolValue.GetValue()? LOCTEXT("IsNotTarget", "Is current AI target") : LOCTEXT("IsTarget", "Is Not current AI target");
}

#undef LOCTEXT_NAMESPACE
