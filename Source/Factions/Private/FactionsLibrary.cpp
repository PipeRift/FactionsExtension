// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "FactionsLibrary.h"
#include <EngineUtils.h>
#include <Engine/Engine.h>

#include "FactionsModule.h"


FFaction UFactionsLibrary::AddFaction(const FName& Name, const FFactionBehavior& Behavior, const FFactionDescriptor& Descriptor)
{
	UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	return Settings->GetFactions().AddFaction(Name, Behavior, Descriptor);
}

void UFactionsLibrary::RemoveFaction(FFaction Faction)
{
	UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	Settings->GetFactions().RemoveFaction(Faction);
}

void UFactionsLibrary::GetAllFactions(TArray<FFaction>& Factions)
{
	const UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	const auto& AllFactions = Settings->GetFactions().Descriptors;

	Factions.Reserve(Factions.Num() + AllFactions.Num());

	for (const auto& Entry : AllFactions)
	{
		Factions.Add({Entry.Key});
	}
}

bool UFactionsLibrary::AddRelation(const FFactionRelation& NewRelation)
{
	UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	return Settings->Internal_AddRelation(NewRelation);
}

bool UFactionsLibrary::RemoveRelation(const FFactionRelation& Relation)
{
	UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	return Settings->Internal_RemoveRelation(Relation);
}

bool UFactionsLibrary::GetAllActorsWithFaction(const UObject* ContextObject, const FFaction Faction, EFactionTestMode Comparison, TSubclassOf<AActor> ActorClass, TArray<AActor*>& OutActors)
{
	QUICK_SCOPE_CYCLE_COUNTER(UGameplayStatics_GetAllActorsOfClass);
	OutActors.Reset();

	if (Faction.IsNone() || !ActorClass)
		return false;

	UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::ReturnNull);
	if (World)
	{
		switch (Comparison) {
		case EFactionTestMode::IsTheSame:
			for (TActorIterator<AActor> It(World, ActorClass); It; ++It)
			{
				AActor* Actor = *It;
				if (IsValid(Actor) && UFactionsLibrary::GetFaction(Actor) == Faction)
					OutActors.Add(Actor);
			}
			break;
		case EFactionTestMode::IsFriendly:
			for (TActorIterator<AActor> It(World, ActorClass); It; ++It)
			{
				AActor* Actor = *It;
				if (IsValid(Actor) && UFactionsLibrary::GetFaction(Actor).IsFriendlyTowards(Faction))
					OutActors.Add(Actor);
			}
			break;
		case EFactionTestMode::IsNeutral:
			for (TActorIterator<AActor> It(World, ActorClass); It; ++It)
			{
				AActor* Actor = *It;
				if (IsValid(Actor) && UFactionsLibrary::GetFaction(Actor).IsNeutralTowards(Faction))
					OutActors.Add(Actor);
			}
			break;
		case EFactionTestMode::IsHostile:
			for (TActorIterator<AActor> It(World, ActorClass); It; ++It)
			{
				AActor* Actor = *It;
				if (IsValid(Actor) && UFactionsLibrary::GetFaction(Actor).IsHostileTowards(Faction))
					OutActors.Add(Actor);
			}
			break;
		}

		return OutActors.Num() > 0;
	}

	return false;
}
