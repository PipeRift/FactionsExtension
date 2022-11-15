// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "FactionsLibrary.h"

#include "FactionsSubsystem.h"

#include <Engine/Engine.h>
#include <EngineUtils.h>


FFaction UFactionsLibrary::AddFaction(const FName& Name, const FFactionDescriptor& Descriptor)
{
	UFactionsSubsystem* Settings = UFactionsSubsystem::Get(GEngine);
	check(Settings);

	return Settings->GetFactions().AddFaction(Name, Descriptor);
}

void UFactionsLibrary::RemoveFaction(FFaction Faction)
{
	// TODO: Move this function to subsystem
	UFactionsSubsystem* Settings = UFactionsSubsystem::Get(GEngine);
	check(Settings);

	Settings->GetFactions().RemoveFaction(Faction);
}

void UFactionsLibrary::GetAllFactions(TArray<FFaction>& Factions)
{
	// TODO: Move this function to subsystem
	UFactionsSubsystem* Settings = UFactionsSubsystem::Get(GEngine);
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
	// TODO: Move this function to subsystem
	UFactionsSubsystem* Settings = UFactionsSubsystem::Get(GEngine);
	check(Settings);

	return Settings->Internal_AddRelation(NewRelation);
}

bool UFactionsLibrary::RemoveRelation(const FFactionRelation& Relation)
{
	// TODO: Move this function to subsystem
	UFactionsSubsystem* Settings = UFactionsSubsystem::Get(GEngine);
	check(Settings);

	return Settings->Internal_RemoveRelation(Relation);
}

bool UFactionsLibrary::GetAllActorsWithFaction(const UObject* ContextObject, const FFaction Faction,
	EFactionTestMode Comparison, TSubclassOf<AActor> ActorClass, TArray<AActor*>& OutActors)
{
	QUICK_SCOPE_CYCLE_COUNTER(UGameplayStatics_GetAllActorsOfClass);
	OutActors.Reset();

	if (Faction.IsNone() || !ActorClass)
		return false;

	UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::ReturnNull);
	if (World)
	{
		switch (Comparison)
		{
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
