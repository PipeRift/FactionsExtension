// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "Asset/FactionCollectionFactory.h"

#include <ClassViewerModule.h>
#include <ClassViewerFilter.h>
#include "Kismet2/SClassPickerDialog.h"
#include "FactionCollection.h"

#define LOCTEXT_NAMESPACE "FactionCollection"


UFactionCollectionFactory::UFactionCollectionFactory()
	: Super()
{
	SupportedClass = UFactionCollection::StaticClass();

	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UFactionCollectionFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if(Class != nullptr)
	{
		// if we have no asset class, use the passed-in class instead
		check(Class == UFactionCollection::StaticClass() || Class->IsChildOf(UFactionCollection::StaticClass()));
		return NewObject<UFactionCollection>(InParent, Class, Name, Flags);
	}
	return nullptr;
}

#undef LOCTEXT_NAMESPACE
