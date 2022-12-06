// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "FactionsSubsystem.h"

#include <EngineUtils.h>


using FBehaviorSort = FNameFastLess;

bool UFactionsSubsystem::hasSetTeamIdAttitudeSolver = false;


UFactionsSubsystem::UFactionsSubsystem() : Super()
{
	AddFaction(TEXT("Default"), {});
}

void UFactionsSubsystem::PostInitProperties()
{
	Super::PostInitProperties();
	BakeFactions();

	if (!IsDefaultSubobject() && !hasSetTeamIdAttitudeSolver)
	{
		FGenericTeamId::SetAttitudeSolver([this](FGenericTeamId A, FGenericTeamId B)
		{
			return GetAttitude(FromTeamId(A), FromTeamId(B));
		});
		hasSetTeamIdAttitudeSolver = true;
	}
}

const FFactionDescriptor* UFactionsSubsystem::FindDescriptor(
	FFaction Faction, UFactionCollection** OutCollection) const
{
	const FFactionDescriptor* Descriptor = GetFactions().GetDescriptor(Faction);
	UFactionCollection* Collection = nullptr;

	for (auto It = Collections.begin(); !Descriptor && It != Collections.end(); ++It)
	{
		Collection = *It;
		if (Collection)
		{
			Descriptor = Collection->Factions.GetDescriptor(Faction);
		}
	}

	if (Descriptor && OutCollection)
	{
		*OutCollection = Collection;
	}
	return nullptr;
}

TEnumAsByte<ETeamAttitude::Type> UFactionsSubsystem::GetAttitude(
	const FFaction Source, const FFaction Target) const
{
	if (const auto* Relation = FindRelation(Source, Target))
	{
		return Relation->Attitude;
	}

	const int32 Index = GetFactionIndex(Source.Id);
	if (BakedBehaviors.IsValidIndex(Index))
	{
		const auto& Behavior = BakedBehaviors[Index];
		return Source == Target ? Behavior.SelfAttitude : Behavior.ExternalAttitude;
	}
	else if (!Source.IsNone())
	{
		UE_LOG(LogFactions, Warning, TEXT("Tried to get an attitude using an invalid faction ('%s'). All factions must be registered in the Factions Subsystem."), *Source.GetId().ToString());
	}
	return ETeamAttitude::Neutral;
}

int32 UFactionsSubsystem::GetFactionIndex(FFaction Faction) const
{
	return Algo::BinarySearchBy(BakedBehaviors, Faction.GetId(), [](const auto& Behavior) {
		return Behavior.Id;
	}, FBehaviorSort{});
}

FFaction UFactionsSubsystem::FromTeamId(FGenericTeamId TeamId) const
{
	const int32 Index = TeamId.GetId();
	if (BakedBehaviors.IsValidIndex(Index))
	{
		return {BakedBehaviors[Index].Id};
	}
	return FFaction::NoFaction();
}

FGenericTeamId UFactionsSubsystem::ToTeamId(FFaction Faction) const
{
	const int32 Index = GetFactionIndex(Faction);
	if (Index != INDEX_NONE &&
		ensureMsgf(Index < FGenericTeamId::NoTeam.GetId(), TEXT("Faction Index exceeded maximum GenericTeamIds. GenericTeamId only supports up to 255 teams")))
	{
		return FGenericTeamId{uint8(Index)};
	}
	return FGenericTeamId::NoTeam;
}

FString UFactionsSubsystem::GetDisplayName(const FFaction Faction) const
{
	if (const auto* Descriptor = FindDescriptor(Faction))
	{
		const bool bUseId = Descriptor->bIdAsDisplayName ||
			(bUseIdsIfDisplayNamesAreEmpty && Descriptor->DisplayName.IsEmpty());
		if (!bUseId)
		{
			return Descriptor->DisplayName.ToString();
		}
	}
	return Faction.GetId().ToString();
}

FFaction UFactionsSubsystem::AddFaction(const FName& Id, const FFactionDescriptor& Descriptor)
{
	if (!Id.IsNone() && !Factions.List.Contains(Id))
	{
		Factions.List.Add(Id, Descriptor);
		AddBakedFaction(Id, Descriptor);
	}
	return {Id};
}

FFaction UFactionsSubsystem::EmplaceFaction(const FName& Id, FFactionDescriptor Descriptor)
{
	if (!Id.IsNone())
	{
		Factions.List.Emplace(Id, MoveTemp(Descriptor));
		AddBakedFaction(Id, Descriptor);
		return {Id};
	}
	return {};
}

void UFactionsSubsystem::RemoveFaction(FFaction Faction)
{
	if (!Faction.IsNone())
	{
		RemoveBakedFaction(Faction);
		Factions.List.Remove(Faction.GetId());
	}
}

bool UFactionsSubsystem::AddRelation(const FFactionRelation& Relation)
{
	if (Relation.IsValid())
	{
		return Relations.List.Add(Relation).IsValidId();
	}
	return false;
}

bool UFactionsSubsystem::RemoveRelation(const FFactionRelation& Relation)
{
	if (Relation.IsValid())
	{
		return Relations.List.Remove(Relation) > 0;
	}
	return false;
}

bool UFactionsSubsystem::AddCollection(UFactionCollection* Collection)
{
	if (Collections.Contains(Collection))
	{
		UE_LOG(LogFactions, Warning, TEXT("Collection ('%s') has already been added before."),
			*Collection->GetName());
		return false;
	}

	Collections.Add(Collection);

	bool bFactionExisted = false;
	for (const auto& Descriptor : Collection->Factions.List)
	{
		AddBakedFaction(Descriptor.Key, Descriptor.Value, &bFactionExisted);
		if (bFactionExisted)
		{
			UE_LOG(LogFactions, Warning,
				TEXT("Added a duplicated faction from a collection. Relation: (%s)"),
				*Descriptor.Key.ToString());
		}
	}

	
	bool bRepeated = false;
	Relations.List.Reserve(Relations.List.Num() + Collection->Relations.List.Num());
	for (const auto& Relation : Collection->Relations.List)
	{
		Relations.List.Add(Relation, &bRepeated);
		if (bRepeated)
		{
			UE_LOG(LogFactions, Warning, TEXT("Added a duplicated relation from a collection. Relation: (%s)"), *Relation.ToString(Collection));
		}
	}
	Relations.List.Append(Collection->Relations.List);

	return true;
}

int32 UFactionsSubsystem::ClearFactions()
{
	const int32 Count = Factions.Num();
	Factions.List.Empty();
	BakedBehaviors.Empty();
	return Count;
}

int32 UFactionsSubsystem::ClearRelations()
{
	const int32 Count = Relations.Num();
	Relations.List.Empty();
	return Count;
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
	if (auto* Found = Factions.GetDescriptor(Faction))
	{
		*Found = Descriptor;

		const int32 Index = GetFactionIndex(Faction);
		if (ensure(Index != INDEX_NONE))
		{
			auto& Baked = BakedBehaviors[Index];
			Baked.SelfAttitude = Found->SelfAttitude;
			Baked.ExternalAttitude = Found->ExternalAttitude;
		}
		return true;
	}
	return false;
}

void UFactionsSubsystem::GetAllFactions(TArray<FFaction>& OutFactions) const
{
	OutFactions.Reserve(OutFactions.Num() + BakedBehaviors.Num());
	for (const auto& Behavior : BakedBehaviors)
	{
		OutFactions.Add({Behavior.Id});
	}
}

bool UFactionsSubsystem::BPFindDescriptor(
	const FFaction Faction, FFactionDescriptor& Descriptor, UFactionCollection*& Collection) const
{
	if (auto* Found = FindDescriptor(Faction, &Collection))
	{
		Descriptor = *Found;
		return true;
	}
	return false;
}

void UFactionsSubsystem::BeginDestroy()
{
	if (hasSetTeamIdAttitudeSolver)
	{
		FGenericTeamId::ResetAttitudeSolver();
		hasSetTeamIdAttitudeSolver = false;
	}
	Super::BeginDestroy();
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


void UFactionsSubsystem::BakeFactions()
{
	// Avoid memory deallocation
	BakedBehaviors.Empty(Factions.Num());
	// Reduce possible memory allocation
	BakedBehaviors.Reserve(Factions.Num());

	for (const auto& It : Factions.List)
	{
		const auto& Descriptor = It.Value;
		BakedBehaviors.Add({
			It.Key,
			Descriptor.SelfAttitude,
			Descriptor.ExternalAttitude
		});
	}
	BakedBehaviors.Sort([](const auto& A, const auto& B){
		return A.Id.FastLess(B.Id);
	});
}

void UFactionsSubsystem::AddBakedFaction(FName Id, const FFactionDescriptor& Descriptor, bool* bWasAlreadyAdded)
{
	// Insert sorted
	const int32 Index = Algo::LowerBoundBy(BakedBehaviors, Id, [](const auto& Behavior) {
		return Behavior.Id;
	}, FBehaviorSort{});
	check(Index >= 0 && Index <= BakedBehaviors.Num());

	const FBakedFactionBehavior Behavior {
		Id,
		Descriptor.SelfAttitude,
		Descriptor.ExternalAttitude
	};
	bool bReplaced = false;
	if (BakedBehaviors.IsValidIndex(Index))
	{
		// Since we returned lower bound we already know Id <= Index key. So if Id is not < Index key, they must be equal
		if (!FBehaviorSort{}(Id, BakedBehaviors[Index].Id))
		{
			// Found, replace
			BakedBehaviors[Index] = Behavior;
			bReplaced = true;
		}
		else
		{
			BakedBehaviors.Insert(Behavior, Index);
		}
	}
	else
	{
		BakedBehaviors.Add(Behavior);
	}

	if (bWasAlreadyAdded)
	{
		*bWasAlreadyAdded = bReplaced;
	}
}

void UFactionsSubsystem::RemoveBakedFaction(FFaction Faction)
{
	const int32 Index = GetFactionIndex(Faction);
	if (Index != INDEX_NONE)
	{
		BakedBehaviors.RemoveAt(Index);
	}
}
