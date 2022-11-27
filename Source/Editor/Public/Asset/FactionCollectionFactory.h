// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "FactionCollection.h"
#include "FactionCollectionFactory.generated.h"


UCLASS()
class UFactionCollectionFactory : public UFactory
{
	GENERATED_BODY()


	UFactionCollectionFactory();

	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End of UFactory interface
};
