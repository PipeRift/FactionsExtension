// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"

#include "Faction.h"
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
	GENERATED_IINTERFACE_BODY()


public:

	/** Retrieve owner attitude towards given Other object */
	virtual const ETeamAttitude::Type GetAttitudeTowards(const AActor& Other) const;

	/** Return if owner is hostile towards given Other object */
	FORCEINLINE const bool IsHostileTowards(const AActor& Other) const {
		return GetAttitudeTowards(Other) == ETeamAttitude::Hostile;
	}

	/** @return the current faction */
	static const FFaction Execute_GetFaction(const AActor* Other);

	/** @param Faction that will be assigned */
	static void Execute_SetFaction(AActor* Other, const FFaction& NewFaction);

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
	UFUNCTION(BlueprintImplementableEvent, Category = Faction, meta = (DisplayName = "Get Faction"))
	void EventGetFaction(FFaction& OutFaction) const;

	/** @param Faction that will be assigned */
	UFUNCTION(BlueprintImplementableEvent, Category = Faction, meta = (DisplayName = "Set Faction"))
	void EventSetFaction(const FFaction& Faction);

private:

	/** Begin GenericTeamAgent interface */

	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override {
		SetFaction(FFaction(TeamID));
	}

	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override {
		return GetFaction().GetTeam();
	}

	/** Retrieved owner attitude toward given Other object */
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override
	{
		return GetAttitudeTowards(Other);
	}

	/** End GenericTeamAgent interface */
};
