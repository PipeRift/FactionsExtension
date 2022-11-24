// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "Faction.h"
#include "FactionsSubsystem.h"
#include "TestHelpers.h"


#define BASE_SPEC FFactionsSpec

BEGIN_TESTSPEC(FFactionRegistrySpec, "FactionsExtension.Registry", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
UFactionsSubsystem* Factions = nullptr;
FFaction FactionA;
FFaction FactionB;
END_TESTSPEC(FFactionRegistrySpec)

void FFactionRegistrySpec::Define()
{
	BeforeEach([this]() {
		Factions = NewObject<UFactionsSubsystem>();
		TestTrue(TEXT("Faction Subsystem is not null"), Factions != nullptr);
		FactionA = Factions->EmplaceFaction("A", {});
		FactionB = Factions->EmplaceFaction("B", {});
	});

	It("Faction can be registered", [this]() {
		TestFalse(TEXT("FactionA is none"), FactionA.IsNone());
		TestFalse(TEXT("FactionB is none"), FactionB.IsNone());
		TestTrue(TEXT("FactionA is valid"), Factions->IsValid(FactionA));
		TestTrue(TEXT("FactionB is valid"), Factions->IsValid(FactionB));
	});

	It("Faction can be unregistered", [this]() {
		TestTrue(TEXT("Faction is valid"), Factions->IsValid(FactionA));
		Factions->RemoveFaction(FactionA);
		TestFalse(TEXT("Faction is valid"), Factions->IsValid(FactionA));
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
