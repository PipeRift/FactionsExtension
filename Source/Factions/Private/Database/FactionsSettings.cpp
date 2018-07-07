// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "FactionsSettings.h"
#include "FactionsModule.h"


UFactionsSettings::UFactionsSettings()
	: Super()
{
	Factions.Add(TEXT("Default"), FFactionInfo(FColor::Blue));

	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UFactionsSettings::OnWorldInitialization);
}

bool UFactionsSettings::Internal_RegistryFaction(const FName& Name, const FFactionInfo& FactionInfo)
{
	// Faction already exists
	if (Factions.Contains(Name))
		return false;

	Factions.Add(Name, FactionInfo);
	return true;
}

bool UFactionsSettings::Internal_UnregistryFaction(FFaction Faction)
{
	if (Faction.IsNone())
		return false;

	Factions.Remove(Faction.GetIdName());
	MarkPackageDirty();
	return true;
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
bool UFactionsSettings::CanEditChange(const UProperty* InProperty) const
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
