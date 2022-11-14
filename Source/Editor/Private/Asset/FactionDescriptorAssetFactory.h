// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "FactionDescriptorAsset.h"
#include "FactionDescriptorAssetFactory.generated.h"


UCLASS()
class UFactionDescriptorAssetFactory : public UFactory
{
	GENERATED_BODY()


	UPROPERTY()
	TSubclassOf<UFactionDescriptorAsset> AssetClass;


	UFactionDescriptorAssetFactory();

	// UFactory interface
	/** For selecting a certain faction info subclass*/
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End of UFactory interface
};
