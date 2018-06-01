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


#if WITH_EDITOR
void UFactionsSettings::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property != nullptr)
	{
		//const FName PropertyName(PropertyChangedEvent.Property->GetFName());

		/*if (PropertyName == GET_MEMBER_NAME_CHECKED(UAIExtensionSettings, Factions))
		{}*/

		auto* Link = PropertyChangedEvent.PropertyChain.GetActiveMemberNode();
		const auto* LinkValue = Link->GetValue();

		if (LinkValue && LinkValue->GetFName() == GET_MEMBER_NAME_CHECKED(UFactionsSettings, Relations))
		{
			const int32 RelationIndex = PropertyChangedEvent.GetArrayIndex(LinkValue->GetFName().ToString());

			SanitizeRelations(PropertyChangedEvent.ChangeType, RelationIndex);
		}
	}
}

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

void UFactionsSettings::SanitizeRelations(EPropertyChangeType::Type ChangeType, int32 RelationIndex)
{
	if (ChangeType == EPropertyChangeType::Interactive)
		return;

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

#endif //WITH_EDITOR
