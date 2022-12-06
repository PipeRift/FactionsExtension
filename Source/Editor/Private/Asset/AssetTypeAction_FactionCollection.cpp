// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "Asset/AssetTypeAction_FactionCollection.h"
#include "FactionCollection.h"
#include "FactionsModule.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"


//////////////////////////////////////////////////////////////////////////
// FAssetTypeAction_FactionCollection

FText FAssetTypeAction_FactionCollection::GetName() const
{
	return LOCTEXT("FAssetTypeAction_FactionCollectionName", "Faction Collection");
}

FColor FAssetTypeAction_FactionCollection::GetTypeColor() const
{
	return FColor(170, 66, 244);
}

UClass* FAssetTypeAction_FactionCollection::GetSupportedClass() const
{
	return UFactionCollection::StaticClass();
}

uint32 FAssetTypeAction_FactionCollection::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
