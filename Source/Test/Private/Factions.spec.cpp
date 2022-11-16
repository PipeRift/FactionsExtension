// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "Faction.h"
#include "FactionsSubsystem.h"
#include "TestHelpers.h"


namespace
{
	constexpr uint32 TestFlags =
		EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter;
}

#define BASE_SPEC FFactionsSpec

BEGIN_TESTSPEC(FFactionRegistrySpec, "FactionsExtension.Registry", TestFlags)
UFactionsSubsystem* Factions = nullptr;
FFaction FactionA;
FFaction FactionB;
END_TESTSPEC(FFactionRegistrySpec)

void FFactionRegistrySpec::Define()
{
	BeforeEach([this]() {
		Factions = NewObject<UFactionsSubsystem>();
		Factions->AddToRoot();
		TestTrue(TEXT("Faction Subsystem is not null"), Factions != nullptr);
		FactionA = Factions->AddFaction("A", {});
		FactionB = Factions->AddFaction("B", {});
	});

	It("Faction can be registered", [this]() {
		TestFalse(TEXT("Faction is not none"), FactionA.IsNone());
	});

	It("Faction can be unregistered", [this]() {
		Factions->RemoveFaction(FactionA);
		TestTrue(TEXT("Faction is none"), FactionA.IsNone());
	});

	It("Relation can be registered", [this]() {
		bool bResult = Factions->AddRelation({FactionA, FactionB});
		TestTrue(TEXT("Registered relation"), bResult);
	});

	It("Relation can be unregistered", [this]() {
		Factions->AddRelation({FactionA, FactionB});

		bool bResult = Factions->RemoveRelation({FactionA, FactionB});
		TestTrue(TEXT("Unregistered relation"), bResult);
	});

	AfterEach([this]() {
		Factions->RemoveFaction(FactionA);
		Factions->RemoveFaction(FactionB);
		Factions->MarkAsGarbage();
		Factions = nullptr;
	});
}
