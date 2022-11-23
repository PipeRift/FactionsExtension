// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Faction.h"
#include "GenericTeamAgentInterface.h"

#include "FactionAgentInterface.generated.h"


// This class does not need to be modified.
UINTERFACE()
class FACTIONS_API UFactionAgentInterface : public UGenericTeamAgentInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 *
 */
class FACTIONS_API IFactionAgentInterface : public IGenericTeamAgentInterface
{
	GENERATED_BODY()


public:
	/** @return the current faction */
	static FFaction GetFaction(const UObject* Source);

	/** @param Faction that will be assigned */
	static void SetFaction(UObject* Source, const FFaction& Faction);


	/** Retrieve owner attitude towards given Target object */
	const ETeamAttitude::Type GetAttitude(const UObject& Target) const;

	/** Return if owner is hostile towards given Other object */
	bool IsFriendly(const AActor& Target) const
	{
		return GetAttitude(Target) == ETeamAttitude::Friendly;
	}
	bool IsNeutral(const AActor& Target) const
	{
		return GetAttitude(Target) == ETeamAttitude::Neutral;
	}
	bool IsHostile(const AActor& Target) const
	{
		return GetAttitude(Target) == ETeamAttitude::Hostile;
	}

protected:
	/** Returns the current faction (Call Execute_GetFaction instead for BP and C++ support)
	 * @return the current faction
	 */
	virtual FFaction GetFaction() const;

	/** Set the current faction (Call Execute_SetFaction instead for BP and C++ support)
	 * @param Faction that will be assigned
	 */
	virtual void SetFaction(const FFaction& Faction);

	/** @return the current faction */
	UFUNCTION(
		BlueprintNativeEvent, Category = Faction, meta = (DisplayName = "Get Faction", CallInEditor = "true"))
	void EventGetFaction(FFaction& OutFaction) const;

	/** @param Faction that will be assigned */
	UFUNCTION(
		BlueprintNativeEvent, Category = Faction, meta = (DisplayName = "Set Faction", CallInEditor = "true"))
	void EventSetFaction(const FFaction& Faction);

private:
	void EventGetFaction_Implementation(FFaction& OutFaction) const
	{
		// By default ask C++ inheritance
		OutFaction = GetFaction();
	}

	void EventSetFaction_Implementation(const FFaction& Faction)
	{
		// By default ask C++ inheritance
		SetFaction(Faction);
	}


	/** Begin GenericTeamAgent interface */

	/** Assigns Team Agent to given TeamID */
	void SetGenericTeamId(const FGenericTeamId& TeamID) override;

	/** Retrieve team identifier in form of FGenericTeamId */
	FGenericTeamId GetGenericTeamId() const override;

	/** Retrieved owner attitude toward given Other object */
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override
	{
		return GetAttitude(Other);
	}

	/** End GenericTeamAgent interface */
};
