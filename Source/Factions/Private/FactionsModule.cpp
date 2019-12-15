// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "FactionsModule.h"
#include "GameplayTagsManager.h"

#include "FactionsSettings.h"
#include "GameDelegates.h"

DEFINE_LOG_CATEGORY(LogFactions)

#define LOCTEXT_NAMESPACE "FactionsModule"


void FFactionsModule::StartupModule()
{
	UE_LOG(LogFactions, Log, TEXT("Factions: Log Started"));

	RegisterSettings();

	CacheFactionInformation();

	OnEndPlayHandle = FGameDelegates::Get().GetEndPlayMapDelegate().AddRaw(this, &FFactionsModule::OnEndPlay);
}

void FFactionsModule::ShutdownModule()
{
	UE_LOG(LogFactions, Log, TEXT("Factions: Log Ended"));

	FGameDelegates::Get().GetEndPlayMapDelegate().Remove(OnEndPlayHandle);

	if (UObjectInitialized())
	{
		UnregisterSettings();

		// Destroy faction Info
		if (FactionManager.IsValid())
		{
			FactionManager->RemoveFromRoot();
			FactionManager->MarkPendingKill();
		}
	}
}

void FFactionsModule::RegisterSettings()
{
#if WITH_EDITOR
	// Registering some settings is just a matter of exposing the default UObject of
	// your desired class, feel free to add here all those settings you want to expose
	// to your LDs or artists.

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		// Get Project Settings category
		ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");

		// Register Factions settings
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Game", "Factions",
			LOCTEXT("RuntimeFactionsSettingsName", "Factions"),
			LOCTEXT("RuntimeFactionsDescription", "Factions database and relations table"),
			GetMutableDefault<UFactionsSettings>());

		// Register the save handler to your settings, you might want to use it to
		// validate those or just act to settings changes.
		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FFactionsModule::HandleSettingsSaved);
		}
	}
#endif
}

void FFactionsModule::UnregisterSettings()
{
#if WITH_EDITOR
	// Ensure to unregister all of your registered settings here, hot-reload would
	// otherwise yield unexpected results.

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Game", "Factions");
	}
#endif
}

bool FFactionsModule::HandleSettingsSaved()
{
	UFactionsSettings* Settings = GetMutableDefault<UFactionsSettings>();
	bool ResaveSettings = false;

	if (ModifiedSettingsDelegate.IsBound()) {
		ModifiedSettingsDelegate.Execute();
	}

	// You can put any validation code in here and resave the settings in case an invalid
	// value has been entered

	if (ResaveSettings)
	{
		Settings->SaveConfig();
	}

	CacheFactionInformation();
	return true;
}

void FFactionsModule::CacheFactionInformation()
{
	// Destroy previous Info
	if (FactionManager.IsValid())
	{
		FactionManager->RemoveFromRoot();
		FactionManager->MarkPendingKill();
	}

	// Create new infos from settings, and don't allow GC destruction
	FactionManager = NewObject<UFactionsSettings>();
	FactionManager->AddToRoot();
}

TWeakObjectPtr<class UFactionsSettings> FFactionsModule::FactionManager {};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FFactionsModule, Factions)
