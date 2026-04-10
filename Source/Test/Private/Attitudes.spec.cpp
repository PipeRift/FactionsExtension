// Copyright 2015-2026 Piperift. All Rights Reserved.

#include "Automatron.h"
#include "Faction.h"
#include "FactionsSubsystem.h"


class FFactionsSpec_Attitudes : public Automatron::FTestSpec
{
	GENERATE_SPEC(FFactionsSpec_Attitudes, "Factions.Attitudes",
		EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask);

	TObjectPtr<UFactionsSubsystem> Factions;
	FFaction FactionA;
	FFaction FactionB;
};


void FFactionsSpec_Attitudes::Define()
{
	BeforeEach([this]() {
		Factions = NewObject<UFactionsSubsystem>();
		TestTrueExpr(Factions != nullptr);
		Factions->ClearRelations();
		FactionA = Factions->EmplaceFaction(
			"A", {/*Self*/ ETeamAttitude::Friendly, /*External*/ ETeamAttitude::Hostile});
		FactionB = Factions->EmplaceFaction(
			"B", {/*Self*/ ETeamAttitude::Hostile, /*External*/ ETeamAttitude::Friendly});
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
