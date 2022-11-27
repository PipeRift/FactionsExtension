// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "Asset/FactionCollectionFactory.h"

#include <ClassViewerModule.h>
#include <ClassViewerFilter.h>
#include "Kismet2/SClassPickerDialog.h"
#include "FactionCollection.h"

#define LOCTEXT_NAMESPACE "FactionCollection"


class FAssetClassParentFilter : public IClassViewerFilter
{
public:
	/** All children of these classes will be included unless filtered out by another setting. */
	TSet< const UClass* > AllowedChildrenOfClasses;

	/** Disallowed class flags. */
	EClassFlags DisallowedClassFlags;

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return !InClass->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return !InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
	}
};


UFactionCollectionFactory::UFactionCollectionFactory()
	: Super()
{
	SupportedClass = UFactionCollection::StaticClass();

	//bText = true;
	bCreateNew = true;
	bEditorImport = true;
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
