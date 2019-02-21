// Copyright 2015-2019 Piperift. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "FactionInfoAsset.h"
#include "FactionInfoAssetFactory.generated.h"


UCLASS()
class UFactionInfoAssetFactory : public UFactory
{
	GENERATED_BODY()


	UPROPERTY()
	TSubclassOf<UFactionInfoAsset> AssetClass;


	UFactionInfoAssetFactory();

	// UFactory interface
	/** For selecting a certain faction info subclass*/
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End of UFactory interface
};
