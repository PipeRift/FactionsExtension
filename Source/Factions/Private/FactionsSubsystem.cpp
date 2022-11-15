// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "FactionsSubsystem.h"

#include "FactionsModule.h"


UFactionsSubsystem::UFactionsSubsystem() : Super()
{
	Factions.AddFaction(TEXT("Default"), {FColor::Blue});

	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UFactionsSubsystem::OnWorldInitialization);
}

bool UFactionsSubsystem::Internal_AddRelation(const FFactionRelation& Relation)
{
	if (!Relation.IsValid())
		return false;

	if (Relations.GetRelations().Add(Relation).IsValidId())
	{
		MarkPackageDirty();
		return true;
	}
	return false;
}

bool UFactionsSubsystem::Internal_RemoveRelation(const FFactionRelation& Relation)
{
	if (!Relation.IsValid())
		return false;

	if (Relations.GetRelations().Remove(Relation) > 0)
	{
		MarkPackageDirty();
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
