// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "EnvQueryTest_Faction.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"

#include "FactionsLibrary.h"

#define LOCTEXT_NAMESPACE "UEnvQueryTest_Faction"


UEnvQueryTest_Faction::UEnvQueryTest_Faction()
	: Super()
	, bCompareTowardsContextActor(false)
{
	TestPurpose = EEnvTestPurpose::Filter;

	Cost = EEnvTestCost::Low;
	SetWorkOnFloatValues(false);
	ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();

	BoolValue.DefaultValue = false;
}

void UEnvQueryTest_Faction::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* Owner = QueryInstance.Owner.Get();
	if (!Owner)
		return;

	BoolValue.BindData(Owner, QueryInstance.QueryID);
	const bool bNegate = BoolValue.GetValue();

	FFaction TargetFaction{ Faction };
	if (bCompareTowardsContextActor)
	{
		// don't support context Location here, it doesn't make any sense
		TArray<AActor*> ContextItems;
		if (!QueryInstance.PrepareContext(Context, ContextItems))
			return;

		if (ContextItems.Num() <= 0)
			TargetFaction = FFaction::NoFaction;
		else
			TargetFaction = UFactionsLibrary::GetFaction(ContextItems[0]);
	}

	switch (Mode)
	{
	case EFactionTestMode::IsTheSame:
		for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
		{
			AActor* PointActor = GetItemActor(QueryInstance, It.GetIndex());

			const bool bIsTheSame = UFactionsLibrary::GetFaction(PointActor) == TargetFaction;
			It.SetScore(TestPurpose, FilterType, bIsTheSame, bNegate);
		}
		break;

	case EFactionTestMode::IsFriendly:
		for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
		{
			AActor* PointActor = GetItemActor(QueryInstance, It.GetIndex());

			const bool bIsFriendly = UFactionsLibrary::GetFaction(PointActor).IsFriendlyTowards(TargetFaction);
			It.SetScore(TestPurpose, FilterType, bIsFriendly, bNegate);
		}
		break;

	case EFactionTestMode::IsNeutral:
		for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
		{
			AActor* PointActor = GetItemActor(QueryInstance, It.GetIndex());

			const bool bIsNeutral = UFactionsLibrary::GetFaction(PointActor).IsNeutralTowards(TargetFaction);
			It.SetScore(TestPurpose, FilterType, bIsNeutral, bNegate);
		}
		break;

	case EFactionTestMode::IsHostile:
		for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
		{
			AActor* PointActor = GetItemActor(QueryInstance, It.GetIndex());

			const bool bIsHostile = UFactionsLibrary::GetFaction(PointActor).IsHostileTowards(TargetFaction);
			It.SetScore(TestPurpose, FilterType, bIsHostile, bNegate);
		}
		break;
	}
}

FText UEnvQueryTest_Faction::GetDescriptionTitle() const
{
	const bool bNegate = BoolValue.GetValue();
	FString ModeDesc;

	if (Mode == EFactionTestMode::IsTheSame)
	{
		ModeDesc = bNegate? TEXT("Doesn't equal ") : TEXT("Equals ");
	}
	else
	{
		ModeDesc = TEXT("");
		if (bNegate)
			ModeDesc += TEXT("Not ");

		switch (Mode)
		{
		case EFactionTestMode::IsFriendly:
			ModeDesc += TEXT("Friendly towards ");
			break;
		case EFactionTestMode::IsNeutral:
			ModeDesc += TEXT("Neutral towards ");
			break;
		case EFactionTestMode::IsHostile:
			ModeDesc += TEXT("Hostile towards ");
			break;
		default:
			break;
		}
	}

	if (bCompareTowardsContextActor)
	{
		ModeDesc += *UEnvQueryTypes::DescribeContext(Context).ToString();
		ModeDesc += TEXT("'s faction");
	}
	else
	{
		ModeDesc += Faction.GetDisplayName();
	}

	return FText::FromString(*ModeDesc);
}

FText UEnvQueryTest_Faction::GetDescriptionDetails() const
{
	return FText::Format(LOCTEXT("DescriptionFormat", "{0} test: {1}"), Super::GetDescriptionTitle(), DescribeFloatTestParams());
}

#undef LOCTEXT_NAMESPACE
