// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "FactionsLibrary.h"
#include "FactionsModule.h"


FFaction UFactionsLibrary::RegistryFaction(const FName& Name, const FFactionInfo& FactionInfo)
{
	UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	return Settings->Internal_RegistryFaction(Name, FactionInfo);
}

bool UFactionsLibrary::UnregistryFaction(FFaction Faction)
{
	UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	return Settings->Internal_UnregistryFaction(Faction);
}

void UFactionsLibrary::GetAllFactions(TArray<FFaction>& Factions)
{
	const UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	Factions.Reserve(Factions.Num() + Settings->Factions.Num());

	for (const auto& Entry : Settings->Factions)
	{
		Factions.Add({Entry.Key});
	}
}
