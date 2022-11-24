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
		TestTrueExpr(Factions != nullptr);
		Factions->ClearRelations();
		FactionA = Factions->EmplaceFaction("A", {/*Self*/ ETeamAttitude::Friendly, /*External*/ ETeamAttitude::Hostile});
		FactionB = Factions->EmplaceFaction("B", {/*Self*/ ETeamAttitude::Hostile,  /*External*/ ETeamAttitude::Friendly});
	});

	It("Can check self attitude", [this]() {
		auto AttitudeA = Factions->GetAttitude(FactionA, FactionA);
		TestTrueExpr(AttitudeA == ETeamAttitude::Friendly);
		auto AttitudeB = Factions->GetAttitude(FactionB, FactionB);
		TestTrueExpr(AttitudeB == ETeamAttitude::Hostile);
	});

	It("Can check external attitude", [this]() {
		auto AttitudeA = Factions->GetAttitude(FactionA, FactionB);
		TestTrueExpr(AttitudeA == ETeamAttitude::Hostile);

		auto AttitudeB = Factions->GetAttitude(FactionB, FactionA);
		TestTrueExpr(AttitudeB == ETeamAttitude::Friendly);
	});

	AfterEach([this]() {
		Factions->RemoveFaction(FactionA);
		Factions->RemoveFaction(FactionB);
		Factions->MarkAsGarbage();
		Factions = nullptr;
	});
}
