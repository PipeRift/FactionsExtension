// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "Faction.h"
#include "FactionsSubsystem.h"
#include "TestHelpers.h"


#define BASE_SPEC FFactionsSpec

BEGIN_TESTSPEC(FAttitudesSpec, "FactionsExtension.Attitudes", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
UFactionsSubsystem* Factions = nullptr;
FFaction FactionA;
FFaction FactionB;
END_TESTSPEC(FAttitudesSpec)

void FAttitudesSpec::Define()
{
	BeforeEach([this]() {
		Factions = NewObject<UFactionsSubsystem>();
		TestTrue(TEXT("Faction Subsystem is not null"), Factions != nullptr);
		FactionA = Factions->AddFaction("A", {});
		FactionB = Factions->AddFaction("B", {});
	});

	It("Can check self attitude", [this]() {
	});

	It("Can check external attitude", [this]() {
	});

	AfterEach([this]() {
		Factions->RemoveFaction(FactionA);
		Factions->RemoveFaction(FactionB);
		Factions->MarkAsGarbage();
		Factions = nullptr;
	});
}
