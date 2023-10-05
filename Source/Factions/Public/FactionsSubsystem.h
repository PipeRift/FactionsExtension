// Copyright 2015-2023 Piperift. All Rights Reserved.

#pragma once

#include "FactionAgentInterface.h"
#include "FactionTable.h"
#include "RelationTable.h"

#include <CoreMinimal.h>
#include <Engine/World.h>
#include <GenericTeamAgentInterface.h>
#include <Subsystems/WorldSubsystem.h>

#include "FactionsSubsystem.generated.h"



UENUM()
enum class EFactionTestMode : uint8
{
	IsSame,
	IsFriendly,
	IsNeutral,
	IsHostile
};


/**
 * Defined the behavior of a faction after it has been backed form a FactionTable
 */
USTRUCT()
struct FACTIONS_API FBakedFactionBehavior
{
	GENERATED_BODY()

	UPROPERTY()
	FName Id;

	/** Attitude this faction will have against itself. Relations can override it. */
	ETeamAttitude::Type SelfAttitude = ETeamAttitude::Friendly;

	/** Attitude this faction will have against others. Relations can override it. */
	ETeamAttitude::Type ExternalAttitude = ETeamAttitude::Neutral;
};


/**
 * The FactionsSubsystem contains the global registry of factions and the attitudes between them
 */
UCLASS(ClassGroup = FactionsExtension, config = Game, defaultconfig, meta = (DisplayName = "Factions"))
class FACTIONS_API UFactionsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY(config, EditAnywhere, SaveGame, Category = Factions)
	FFactionTable Factions;

	UPROPERTY(config, EditAnywhere, SaveGame, Category = Factions)
	FRelationTable Relations;

	/** If any faction's DisplayName is empty, use the Id instead */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Factions)
	bool bUseIdsIfDisplayNamesAreEmpty = true;

	/**
	 * Baked list of faction behaviors for faster look-up performance.
	 * Always sorted by Id (FName). Index of array equals GenericTeamId.
	 */
	UPROPERTY(Transient)
	TArray<FBakedFactionBehavior> BakedBehaviors;

	static bool hasSetTeamIdAttitudeSolver;


public:
	UFactionsSubsystem();
	void PostInitProperties() override;


	const FFactionRelation* FindRelation(const FFaction& A, const FFaction& B) const
	{
		return Relations.List.Find({A, B});
	}

	FFactionTable& GetFactions()
	{
		return Factions;
	}
	const FFactionTable& GetFactions() const
	{
		return Factions;
	}

	static UFactionsSubsystem* Get(const UObject* ContextObject);

	const FFactionDescriptor* GetDescriptor(FFaction Faction) const;

	/** C++ ONLY API */

	static FFaction GetFaction(const UObject* Source);
	static void SetFaction(UObject* Target, FFaction Faction);
	ETeamAttitude::Type GetAttitude(const UObject* Source, const UObject* Target) const;
	bool IsHostile(const UObject* Source, const UObject* Target) const;
	bool IsFriendly(const UObject* Source, const UObject* Target) const;
	bool IsNeutral(const UObject* Source, const UObject* Target) const;

	int32 GetFactionIndex(FFaction Faction) const;

	FFaction FromTeamId(FGenericTeamId TeamId) const;
	FGenericTeamId ToTeamId(FFaction Faction) const;


	/** BLUEPRINTS & C++ API */

	/** @return Attitude of Source towards Target */
	UFUNCTION(BlueprintPure, Category = Factions)
	TEnumAsByte<ETeamAttitude::Type> GetAttitude(const FFaction Source, const FFaction Target) const;

	/** @return true if Source is Hostile to Target */
	UFUNCTION(BlueprintPure, Category = Factions)
	bool IsHostile(const FFaction Source, const FFaction Target) const;

	/** @return true if Source is Friendly to Target */
	UFUNCTION(BlueprintPure, Category = Factions)
	bool IsFriendly(const FFaction Source, const FFaction Target) const;

	/** @return true if Source is Neutral to Target */
	UFUNCTION(BlueprintPure, Category = Factions)
	bool IsNeutral(const FFaction Source, const FFaction Target) const;

	/**
	 * Replace the information of an existing faction
	 * @param Faction to change
	 * @param Descriptor to assign
	 * @return true if the faction was found and modified
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	bool SetDescriptor(const FFaction Faction, const FFactionDescriptor& Descriptor);

	/** Return all currently registered factions */
	UFUNCTION(BlueprintCallable, Category = Factions, meta = (DisplayName = "Get All Factions"))
	void GetAllFactions(TArray<FFaction>& OutFactions) const;

	/** Return true if 'Faction' is registered */
	UFUNCTION(BlueprintPure, Category = Factions)
	bool IsValid(FFaction Faction) const;

	UFUNCTION(BlueprintPure, Category = Factions)
	FString GetDisplayName(const FFaction Faction) const;

	/** Return true if One and Other have the same faction */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DefaultToSelf = "A"))
	static bool ShareFaction(const UObject* A, const UObject* B);

	/**
	 * Get all actors in the world with a certain faction
	 * @param Faction to check actors for
	 * @param Comparison relation between the actor and the faction
	 * @param ActorClass limits the actors to be found by that class
	 * @return true if operation was successful and the array of actors
	 */
	UFUNCTION(BlueprintCallable, Category = Factions,
		meta = (AdvancedDisplay = "ActorClass", DeterminesOutputType = "ActorClass",
			DynamicOutputParam = "OutActors"))
	bool GetActorsByFaction(const FFaction Faction, TArray<AActor*>& OutActors,
		TSubclassOf<AActor> ActorClass, EFactionTestMode Comparison = EFactionTestMode::IsSame) const;


	/**
	 * Adds a faction and sets its descriptor unless it already existed.
	 * @param Descriptor containing information about the faction
	 * @return the faction created
	 * @see EmplaceFaction, RemoveFaction
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	FFaction AddFaction(const FName& Id, const FFactionDescriptor& Descriptor);

	/**
	 * Adds a faction and sets its descriptor even if it already existed.
	 * @param Descriptor containing information about the faction
	 * @return the faction created or found
	 * @see AddFaction, RemoveFaction
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	FFaction EmplaceFaction(const FName& Id, FFactionDescriptor Descriptor);

	/**
	 * Remove a faction
	 * @param Faction to remove
	 * @return true if the faction was removed
	 * @see AddFaction, EmplaceFaction
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	void RemoveFaction(FFaction Faction);

	/**
	 * Add a relation between two factions
	 * @param Relation to be added
	 * @return true if the relation was registered, false if the two factions were the same or invalid.
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	bool AddRelation(const FFactionRelation& Relation);

	/**
	 * Remove a relation between two factions
	 * @param Relation to be removed
	 * @return true if the relation was unregistered
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	bool RemoveRelation(const FFactionRelation& Relation);

	/**
	 * Removes all factions
	 * @return number of removed factions
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	int32 ClearFactions();

	/**
	 * Removes all relations
	 * @return number of removed relations
	 */
	UFUNCTION(BlueprintCallable, Category = Factions)
	int32 ClearRelations();

	/** Removes all factions and relations */
	UFUNCTION(BlueprintCallable, Category = Factions)
	void Reset();


	/** BLUEPRINT ONLY API */

	/** Return true if two factions are the same */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DisplayName = "Equals", CompactNodeTitle = "=="))
	static bool BPEquals(const FFaction A, const FFaction B);

	/** Return true if two factions are not the same */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DisplayName = "Not Equal", CompactNodeTitle = "!="))
	static bool BPNotEqual(const FFaction A, const FFaction B);

	/** Return true if this faction is none */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DisplayName = "Is None", BlueprintAutocast))
	static bool BPIsNone(const FFaction Faction);

	/**
	 * Find the information of a faction
	 * @param Faction to search for
	 * @param Descriptor of the faction, if found
	 * @return true if the faction was valid and information was found
	 */
	UFUNCTION(BlueprintPure, Category = Factions, meta = (DisplayName = "Get Descriptor"))
	bool BPGetDescriptor(const FFaction Faction, FFactionDescriptor& Descriptor) const;

	/** Return the faction of an actor. None if the actor doesn't implement FactionAgentInterface */
	UFUNCTION(
		BlueprintPure, Category = Factions, meta = (DefaultToSelf = "Source", DisplayName = "Get Faction"))
	static FFaction BPGetObjectFaction(const UObject* Source)
	{
		return GetFaction(Source);
	}

	/** Sets the faction of an actor. Won't apply if the actor doesn't implement FactionAgentInterface.
	 * @param Target actor that will receive the new faction
	 * @param Faction that will be set
	 */
	UFUNCTION(BlueprintCallable, Category = Factions,
		meta = (DefaultToSelf = "Target", DisplayName = "Set Faction"))
	static void BPSetObjectFaction(UObject* Target, FFaction Faction)
	{
		SetFaction(Target, Faction);
	}

	/** @return Attitude of Source's faction towards Target's faction */
	UFUNCTION(BlueprintPure, Category = Factions,
		meta = (DefaultToSelf = "Source", DisplayName = "Get Attitude (Object)"))
	TEnumAsByte<ETeamAttitude::Type> BPGetObjectAttitude(const UObject* Source, const UObject* Target) const
	{
		return GetAttitude(Source, Target);
	}

	/** @return true if Source is Hostile to Target */
	UFUNCTION(BlueprintPure, Category = Factions,
		meta = (DefaultToSelf = "Source", DisplayName = "Is Hostile (Object)"))
	bool BPIsObjectHostile(const UObject* Source, const UObject* Target)
	{
		return IsHostile(Source, Target);
	}

	/** @return true if Source is Friendly to Target */
	UFUNCTION(BlueprintPure, Category = Factions,
		meta = (DefaultToSelf = "Source", DisplayName = "Is Friendly (Object)"))
	bool BPIsObjectFriendly(const UObject* Source, const UObject* Target)
	{
		return IsFriendly(Source, Target);
	}

	/** @return true if One is Neutral to Target */
	UFUNCTION(BlueprintPure, Category = Factions,
		meta = (DefaultToSelf = "Source", DisplayName = "Is Neutral (Object)"))
	bool BPIsObjectNeutral(const UObject* Source, const UObject* Target)
	{
		return IsNeutral(Source, Target);
	}


protected:
	virtual void BeginDestroy() override;

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

	void BakeFactions();
	void AddBakedFaction(FName Id, const FFactionDescriptor& Descriptor);
	void RemoveBakedFaction(FFaction Faction);
};


inline bool UFactionsSubsystem::IsHostile(const FFaction Source, const FFaction Target) const
{
	return GetAttitude(Source, Target) == ETeamAttitude::Hostile;
}

inline bool UFactionsSubsystem::IsFriendly(const FFaction Source, const FFaction Target) const
{
	return GetAttitude(Source, Target) == ETeamAttitude::Friendly;
}

inline bool UFactionsSubsystem::IsNeutral(const FFaction Source, const FFaction Target) const
{
	return GetAttitude(Source, Target) == ETeamAttitude::Neutral;
}


inline FFaction UFactionsSubsystem::GetFaction(const UObject* Source)
{
	return IFactionAgentInterface::GetFaction(Source);
}

inline void UFactionsSubsystem::SetFaction(UObject* Target, FFaction Faction)
{
	IFactionAgentInterface::SetFaction(Target, Faction);
}

inline ETeamAttitude::Type UFactionsSubsystem::GetAttitude(const UObject* Source, const UObject* Target) const
{
	return GetAttitude(GetFaction(Source), GetFaction(Target));
}

inline bool UFactionsSubsystem::IsHostile(const UObject* Source, const UObject* Target) const
{
	return GetAttitude(Source, Target) == ETeamAttitude::Hostile;
}

inline bool UFactionsSubsystem::IsFriendly(const UObject* Source, const UObject* Target) const
{
	return GetAttitude(Source, Target) == ETeamAttitude::Friendly;
}

inline bool UFactionsSubsystem::IsNeutral(const UObject* Source, const UObject* Target) const
{
	return GetAttitude(Source, Target) == ETeamAttitude::Neutral;
}


inline bool UFactionsSubsystem::ShareFaction(const UObject* A, const UObject* B)
{
	return GetFaction(A) == GetFaction(B);
}

inline bool UFactionsSubsystem::IsValid(FFaction Faction) const
{
	return !Faction.IsNone() && Factions.List.Contains(Faction.GetId());
}

inline void UFactionsSubsystem::Reset()
{
	ClearRelations();
	ClearFactions();
}

inline bool UFactionsSubsystem::BPEquals(const FFaction A, const FFaction B)
{
	return A == B;
}

inline bool UFactionsSubsystem::BPNotEqual(const FFaction A, const FFaction B)
{
	return A != B;
}

inline bool UFactionsSubsystem::BPIsNone(const FFaction Faction)
{
	return Faction.IsNone();
}

inline UFactionsSubsystem* UFactionsSubsystem::Get(const UObject* ContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::ReturnNull))
	{
		return UWorld::GetSubsystem<UFactionsSubsystem>(World);
	}
	return nullptr;
}
