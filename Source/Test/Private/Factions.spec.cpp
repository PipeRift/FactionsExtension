// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "TestHelpers.h"
#include "Faction.h"
#include "FactionsLibrary.h"


namespace
{
	constexpr uint32 TestFlags = EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter;
}

#define BASE_SPEC FFactionsSpec

BEGIN_TESTSPEC(FFactionRegistrySpec, "FactionsExtension.Registry", TestFlags)
	FFaction FactionA;
	FFaction FactionB;
END_TESTSPEC(FFactionRegistrySpec)

void FFactionRegistrySpec::Define()
{
	BeforeEach([this]() {
		FactionA = UFactionsLibrary::RegistryFaction("A", {});
		FactionB = UFactionsLibrary::RegistryFaction("B", {});
	});

	It("Faction can be registered", [this]()
	{
		TestFalse(TEXT("Faction is not none"), FactionA.IsNone());
	});

	It("Faction can be unregistered", [this]()
	{
		UFactionsLibrary::UnregistryFaction(FactionA);
		TestTrue(TEXT("Faction is none"), FactionA.IsNone());
	});

	It("Relation can be registered", [this]()
	{
		bool bResult = UFactionsLibrary::RegistryRelation({ FactionA, FactionB });
		TestTrue(TEXT("Registered relation"), bResult);
	});

	It("Relation can be unregistered", [this]()
	{
		UFactionsLibrary::RegistryRelation({ FactionA, FactionB });

		bool bResult = UFactionsLibrary::UnregistryRelation({ FactionA, FactionB });
		TestTrue(TEXT("Unregistered relation"), bResult);
	});

	AfterEach([this]() {
		UFactionsLibrary::UnregistryFaction(FactionA);
		UFactionsLibrary::UnregistryFaction(FactionB);
	});
}
