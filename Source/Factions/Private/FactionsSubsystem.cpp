// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "FactionsSubsystem.h"

#include <EngineUtils.h>


UFactionsSubsystem::UFactionsSubsystem() : Super()
{
	AddFaction(TEXT("Default"), {FColor::Blue});

	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UFactionsSubsystem::OnWorldInitialization);
}

const FFactionDescriptor* UFactionsSubsystem::GetDescriptor(FFaction Faction) const
{
	return GetFactions().GetDescriptor(Faction);
}

TEnumAsByte<ETeamAttitude::Type> UFactionsSubsystem::GetAttitude(
	const FFaction Source, const FFaction Target) const
{
	if (const auto* Relation = FindRelation(Source, Target))
	{
		return Relation->Attitude;
	}

	if (const auto* Behavior = GetDescriptor(Source))
	{
		return Source == Target ? Behavior->SelfAttitude : Behavior->ExternalAttitude;
	}
	return ETeamAttitude::Neutral;
}

FString UFactionsSubsystem::GetDisplayName(const FFaction Faction) const
{
	const auto* Descriptor = GetDescriptor(Faction);
	if (Descriptor && !Descriptor->DisplayName.IsEmpty())
	{
		return Descriptor->DisplayName.ToString();
	}
	return Faction.ToString();
}

FFaction UFactionsSubsystem::AddFaction(const FName& Id, const FFactionDescriptor& Descriptor)
{
	if (!Id.IsNone() && !Factions.Descriptors.Contains(Id))
	{
		Factions.Descriptors.Add(Id, Descriptor);
		return {Id};
	}
	return {};
}

FFaction UFactionsSubsystem::EmplaceFaction(const FName& Id, FFactionDescriptor Descriptor)
{
	if (!Id.IsNone())
	{
		Factions.Descriptors.Emplace(Id, MoveTemp(Descriptor));
		return {Id};
	}
	return {};
}

void UFactionsSubsystem::RemoveFaction(FFaction Faction)
{
	if (!Faction.IsNone())
	{
		Factions.Descriptors.Remove(Faction.GetId());
	}
}

bool UFactionsSubsystem::AddRelation(const FFactionRelation& Relation)
{
	if (Relation.IsValid())
	{
		return Relations.GetRelations().Add(Relation).IsValidId();
	}
	return false;
}

bool UFactionsSubsystem::RemoveRelation(const FFactionRelation& Relation)
{
	if (Relation.IsValid())
	{
		return Relations.GetRelations().Remove(Relation) > 0;
	}
	return false;
}

bool UFactionsSubsystem::GetActorsByFaction(const FFaction Faction, TArray<AActor*>& OutActors,
	TSubclassOf<AActor> ActorClass, EFactionTestMode Comparison) const
{
	QUICK_SCOPE_CYCLE_COUNTER(UFactionsSubsystem_GetActorsByFaction);
	OutActors.Reset();

	UWorld* World = GetWorld();

	if (Faction.IsNone() || !ActorClass || !World)
	{
		return false;
	}

	auto IterateActors = [this, World, &ActorClass, &OutActors](auto Callback) {
		for (TActorIterator<AActor> It(World, ActorClass); It; ++It)
		{
			AActor* Actor = *It;
			check(Actor);

			FFaction ActorFaction = GetFaction(Actor);
			if (Callback(ActorFaction))
			{
				OutActors.Add(Actor);
			}
		}
	};
	switch (Comparison)
	{
		case EFactionTestMode::IsSame:
			IterateActors([Faction](auto ActorFaction) {
				return ActorFaction == Faction;
			});
			break;
		case EFactionTestMode::IsFriendly:
			IterateActors([this, Faction](auto ActorFaction) {
				return IsFriendly(ActorFaction, Faction);
			});
			break;
		case EFactionTestMode::IsNeutral:
			IterateActors([this, Faction](auto ActorFaction) {
				return IsNeutral(ActorFaction, Faction);
			});
			break;
		case EFactionTestMode::IsHostile:
			IterateActors([this, Faction](auto ActorFaction) {
				return IsHostile(ActorFaction, Faction);
			});
			break;
	}

	return OutActors.Num() > 0;
}

bool UFactionsSubsystem::SetDescriptor(const FFaction Faction, const FFactionDescriptor& Descriptor)
{
	if (auto* Found = GetFactions().GetDescriptor(Faction))
	{
		*Found = Descriptor;
		return true;
	}
	return false;
}

void UFactionsSubsystem::GetAllFactions(TArray<FFaction>& OutFactions) const
{
	const auto& AllFactions = GetFactions().Descriptors;

	OutFactions.Reserve(OutFactions.Num() + AllFactions.Num());
	for (const auto& Entry : AllFactions)
	{
		OutFactions.Add({Entry.Key});
	}
}

bool UFactionsSubsystem::BPGetDescriptor(const FFaction Faction, FFactionDescriptor& Descriptor) const
{
	if (auto* Found = GetFactions().GetDescriptor(Faction))
	{
		Descriptor = *Found;
		return true;
	}
	return false;
}

void UFactionsSubsystem::BeginDestroy()
{
	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
	Super::BeginDestroy();
}

void UFactionsSubsystem::OnWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
{
	Relations.RefreshIndexCache();
}

#if WITH_EDITOR
bool UFactionsSubsystem::CanEditChange(const FProperty* InProperty) const
{
	bool bCanEdit = Super::CanEditChange(InProperty);

	FName PropertyName = InProperty ? InProperty->GetFName() : NAME_None;

	if (GET_MEMBER_NAME_CHECKED(FFactionRelation, Attitude) == PropertyName)
	{
		// TODO: Limit attitude depending on Faction
		bCanEdit &= true;
	}

	return bCanEdit;
}

#endif	  // WITH_EDITOR
