// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "FactionsSettings.h"
#include "FactionsModule.h"


UFactionsSettings::UFactionsSettings()
	: Super()
{
	FactionList.RegistryFaction(TEXT("Default"), { FColor::Blue });

	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UFactionsSettings::OnWorldInitialization);
}

bool UFactionsSettings::Internal_RegistryRelation(const FFactionRelation& Relation)
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

bool UFactionsSettings::Internal_UnregistryRelation(const FFactionRelation& Relation)
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

void UFactionsSettings::BeginDestroy()
{
	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);

	Super::BeginDestroy();
}

void UFactionsSettings::OnWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
{
	//Initialize manager
	FFactionsModule::GetFactionManager();

	//Index relations
	Relations.IndexRelations();
}

#if WITH_EDITOR
bool UFactionsSettings::CanEditChange(const FProperty* InProperty) const
{
	bool bCanEdit = Super::CanEditChange(InProperty);

	FName PropertyName = InProperty ? InProperty->GetFName() : NAME_None;

	if (GET_MEMBER_NAME_CHECKED(FFactionRelation, Attitude) == PropertyName)
	{
		//TODO: Limit attitude depending on Faction
		bCanEdit &= true;
	}

	return bCanEdit;
}

#endif //WITH_EDITOR
