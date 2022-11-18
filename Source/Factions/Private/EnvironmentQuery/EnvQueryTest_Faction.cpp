// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "EnvQueryTest_Faction.h"

#include <EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h>


#define LOCTEXT_NAMESPACE "UEnvQueryTest_Faction"

UEnvQueryTest_Faction::UEnvQueryTest_Faction() : Super(), bCompareTowardsContextActor(false)
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
	const auto* Factions = UFactionsSubsystem::Get(Owner);
	if (!Owner || !Factions)
		return;

	BoolValue.BindData(Owner, QueryInstance.QueryID);
	const bool bNegate = BoolValue.GetValue();

	FFaction TargetFaction{Faction};
	if (bCompareTowardsContextActor)
	{
		// don't support context Location here, it doesn't make any sense
		TArray<AActor*> ContextItems;
		if (!QueryInstance.PrepareContext(Context, ContextItems))
			return;

		if (ContextItems.Num() <= 0)
			TargetFaction = FFaction::NoFaction;
		else
			TargetFaction = UFactionsSubsystem::GetFaction(ContextItems[0]);
	}

	auto IteratePoints = [this, &QueryInstance, bNegate](auto Callback) {
		for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
		{
			AActor* PointActor = GetItemActor(QueryInstance, It.GetIndex());
			FFaction PointFaction = UFactionsSubsystem::GetFaction(PointActor);

			It.SetScore(TestPurpose, FilterType, Callback(PointFaction), bNegate);
		}
	};
	switch (Mode)
	{
		case EFactionTestMode::IsSame:
			IteratePoints([TargetFaction](auto PointFaction) {
				return PointFaction == TargetFaction;
			});
			break;

		case EFactionTestMode::IsFriendly:
			IteratePoints([TargetFaction, Factions](auto PointFaction) {
				return Factions->IsFriendly(PointFaction, TargetFaction);
			});
			break;

		case EFactionTestMode::IsNeutral:
			IteratePoints([TargetFaction, Factions](auto PointFaction) {
				return Factions->IsNeutral(PointFaction, TargetFaction);
			});
			break;

		case EFactionTestMode::IsHostile:
			IteratePoints([TargetFaction, Factions](auto PointFaction) {
				return Factions->IsHostile(PointFaction, TargetFaction);
			});
			break;
	}
}

FText UEnvQueryTest_Faction::GetDescriptionTitle() const
{
	const bool bNegate = BoolValue.GetValue();
	FString ModeDesc;

	if (Mode == EFactionTestMode::IsSame)
	{
		ModeDesc = bNegate ? TEXT("Doesn't equal ") : TEXT("Equals ");
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
		ModeDesc += GetDefault<UFactionsSubsystem>()->GetDisplayName(Faction);
	}

	return FText::FromString(*ModeDesc);
}

FText UEnvQueryTest_Faction::GetDescriptionDetails() const
{
	return FText::Format(LOCTEXT("DescriptionFormat", "{0} test: {1}"), Super::GetDescriptionTitle(),
		DescribeFloatTestParams());
}

#undef LOCTEXT_NAMESPACE
