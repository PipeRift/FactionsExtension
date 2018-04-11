// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"

#include "FactionInfo.h"
#include "FactionsSettings.generated.h"


/**
 * Find Custom Config documentation here: wiki.unrealengine.com/CustomSettings 
 */
UCLASS(config = Game, defaultconfig)
class FACTIONS_API UFactionsSettings : public UObject
{
    GENERATED_BODY()
    
public:

    UFactionsSettings(const FObjectInitializer& ObjectInitializer)
        : Super(ObjectInitializer)
    {
        Factions.Add(FFactionInfo("Default", FColor::Blue));
    }

    UPROPERTY(config, EditAnywhere, Category = Custom)
    TArray<FFactionInfo> Factions;

    UPROPERTY(config, EditAnywhere, Category = Custom)
    TArray<FFactionRelation> Relations; //Moved from Set, serializing caused problems (4.16)

protected:

#if WITH_EDITOR
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
    virtual bool CanEditChange(const UProperty* InProperty) const override;

    void SanitizeRelations(EPropertyChangeType::Type ChangeType, int32 RelationIndex = INDEX_NONE);
#endif
};
