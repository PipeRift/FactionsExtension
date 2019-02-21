// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "Asset/AssetTypeAction_FactionInfoAsset.h"
#include "FactionInfoAsset.h"
#include "FactionsModule.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"


//////////////////////////////////////////////////////////////////////////
// FAssetTypeAction_FactionInfoAsset

FText FAssetTypeAction_FactionInfoAsset::GetName() const
{
	return LOCTEXT("FAssetTypeAction_FactionAssetName", "Faction Info");
}

FColor FAssetTypeAction_FactionInfoAsset::GetTypeColor() const
{
	return FColor(170, 66, 244);
}

UClass* FAssetTypeAction_FactionInfoAsset::GetSupportedClass() const
{
	return UFactionInfoAsset::StaticClass();
}

uint32 FAssetTypeAction_FactionInfoAsset::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
