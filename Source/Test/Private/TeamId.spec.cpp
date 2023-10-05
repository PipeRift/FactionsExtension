// Copyright 2015-2023 Piperift. All Rights Reserved.

#include "Faction.h"
#include "FactionsSubsystem.h"
#include "TestHelpers.h"


#define BASE_SPEC FFactionsSpec

BEGIN_TESTSPEC(FTeamIdSpec, "FactionsExtension.TeamId",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
UFactionsSubsystem* Factions = nullptr;
FFaction FactionA;
FFaction FactionB;
END_TESTSPEC(FTeamIdSpec)

void FTeamIdSpec::Define()
{
	BeforeEach([this]() {
		Factions = NewObject<UFactionsSubsystem>();
		TestTrue(TEXT("Faction Subsystem is not null"), Factions != nullptr);
		FactionA = Factions->EmplaceFaction("A", {});
		FactionB = Factions->EmplaceFaction("B", {});
	});

	It("No Faction equals no Team Id", [this]() {
		FGenericTeamId TeamId = Factions->ToTeamId(FFaction::NoFaction());
		TestTrue(TEXT("TeamId is Invalid"), TeamId == FGenericTeamId::NoTeam.GetId());

		FFaction Faction = Factions->FromTeamId(FGenericTeamId::NoTeam);
		TestTrue(TEXT("Faction is Invalid"), Faction.IsNone());
	});

	It("TeamIds equal Faction Index", [this]() {
		FGenericTeamId TeamIdA = Factions->ToTeamId(FactionA);
		TestTrue(TEXT("TeamIdA is Valid"), TeamIdA.GetId() != FGenericTeamId::NoTeam.GetId());
		TestTrue(TEXT("TeamIdA is Invalid"), TeamIdA.GetId() == Factions->GetFactionIndex(FactionA));

		FGenericTeamId TeamIdB = Factions->ToTeamId(FactionB);
		TestTrue(TEXT("TeamIdB is Valid"), TeamIdB.GetId() != FGenericTeamId::NoTeam.GetId());
		TestTrue(TEXT("TeamIdB is Invalid"), TeamIdB.GetId() == Factions->GetFactionIndex(FactionB));

		TestTrue(TEXT("TeamIdA is not TeamIdB"), TeamIdA.GetId() != TeamIdB.GetId());
	});

	// TODO
	// It("Exceeding factions have no Team Id", [this]() {
	//});

	AfterEach([this]() {
		Factions->RemoveFaction(FactionA);
		Factions->RemoveFaction(FactionB);
		Factions->MarkAsGarbage();
		Factions = nullptr;
	});
}
