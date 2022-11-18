// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "Asset/AssetTypeAction_FactionDescriptorAsset.h"
#include "FactionDescriptorAsset.h"
#include "FactionsModule.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"


//////////////////////////////////////////////////////////////////////////
// FAssetTypeAction_FactionDescriptorAsset

FText FAssetTypeAction_FactionDescriptorAsset::GetName() const
{
	return LOCTEXT("FAssetTypeAction_FactionAssetName", "Faction Descriptor");
}

FColor FAssetTypeAction_FactionDescriptorAsset::GetTypeColor() const
{
	return FColor(170, 66, 244);
}

UClass* FAssetTypeAction_FactionDescriptorAsset::GetSupportedClass() const
{
	return UFactionDescriptorAsset::StaticClass();
}

uint32 FAssetTypeAction_FactionDescriptorAsset::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
