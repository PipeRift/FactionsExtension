// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "Asset/FactionInfoAssetFactory.h"

#include <ClassViewerModule.h>
#include <ClassViewerFilter.h>
#include "Kismet2/SClassPickerDialog.h"
#include "FactionInfoAsset.h"

#define LOCTEXT_NAMESPACE "FactionInfoAsset"


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


UFactionInfoAssetFactory::UFactionInfoAssetFactory()
	: Super()
{
	SupportedClass = UFactionInfoAsset::StaticClass();

	//bText = true;
	bCreateNew = true;
	bEditorImport = true;
	bEditAfterNew = true;
}

bool UFactionInfoAssetFactory::ConfigureProperties()
{
	// nullptr the AssetClass so we can check for selection
	AssetClass = nullptr;

	// Load the class viewer module to display a class picker
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	// Fill in options
	FClassViewerInitializationOptions Options {};
	Options.Mode = EClassViewerMode::ClassPicker;

	TSharedPtr<FAssetClassParentFilter> Filter = MakeShared<FAssetClassParentFilter>();
	Options.ClassFilter = Filter;

	Filter->DisallowedClassFlags = CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists;
	Filter->AllowedChildrenOfClasses.Add(UFactionInfoAsset::StaticClass());

	const FText TitleText = LOCTEXT("CreateItemAssetOptions", "Pick Faction Info Type");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UFactionInfoAsset::StaticClass());

	if (bPressedOk)
	{
		AssetClass = ChosenClass;
	}

	return bPressedOk;
}

UObject* UFactionInfoAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (AssetClass != nullptr)
	{
		return NewObject<UFactionInfoAsset>(InParent, AssetClass, Name, Flags);
	}
	else if(Class != nullptr)
	{
		// if we have no asset class, use the passed-in class instead
		check(Class == UFactionInfoAsset::StaticClass() || Class->IsChildOf(UFactionInfoAsset::StaticClass()));
		return NewObject<UFactionInfoAsset>(InParent, Class, Name, Flags);
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
