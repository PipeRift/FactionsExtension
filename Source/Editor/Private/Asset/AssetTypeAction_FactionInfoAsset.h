// Copyright 2015-2019 Piperift. All Rights Reserved.

#pragma once

#include "AssetTypeActions_Base.h"

class FACTIONSEDITOR_API FAssetTypeAction_FactionInfoAsset : public FAssetTypeActions_Base
{
public:
	// IAssetTypeActions interface
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return false; }
	virtual uint32 GetCategories() override;
	// End of IAssetTypeActions interface

};
