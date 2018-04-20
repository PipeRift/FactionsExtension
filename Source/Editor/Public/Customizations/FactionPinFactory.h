#pragma once
#include "SlateBasics.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphUtilities.h"

#include "Factions/Faction.h"
#include "Private/Customizations/FactionPin.h"
 
class FFactionPinFactory : public FGraphPanelPinFactory
{
    virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override;
};
