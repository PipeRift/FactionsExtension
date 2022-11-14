// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "Asset/FactionDescriptorAssetFactory.h"

#include <ClassViewerModule.h>
#include <ClassViewerFilter.h>
#include "Kismet2/SClassPickerDialog.h"
#include "FactionDescriptorAsset.h"

#define LOCTEXT_NAMESPACE "FactionDescriptorAsset"


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


UFactionDescriptorAssetFactory::UFactionDescriptorAssetFactory()
	: Super()
{
	SupportedClass = UFactionDescriptorAsset::StaticClass();

	//bText = true;
	bCreateNew = true;
	bEditorImport = true;
	bEditAfterNew = true;
}

bool UFactionDescriptorAssetFactory::ConfigureProperties()
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
	Filter->AllowedChildrenOfClasses.Add(UFactionDescriptorAsset::StaticClass());

	const FText TitleText = LOCTEXT("CreateItemAssetOptions", "Pick Faction Descriptor Type");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UFactionDescriptorAsset::StaticClass());

	if (bPressedOk)
	{
		AssetClass = ChosenClass;
	}

	return bPressedOk;
}

UObject* UFactionDescriptorAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (AssetClass != nullptr)
	{
		return NewObject<UFactionDescriptorAsset>(InParent, AssetClass, Name, Flags);
	}
	else if(Class != nullptr)
	{
		// if we have no asset class, use the passed-in class instead
		check(Class == UFactionDescriptorAsset::StaticClass() || Class->IsChildOf(UFactionDescriptorAsset::StaticClass()));
		return NewObject<UFactionDescriptorAsset>(InParent, Class, Name, Flags);
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
