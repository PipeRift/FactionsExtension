// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "AIExtensionPrivatePCH.h"
#include "AIExtensionSettings.h"


#if WITH_EDITOR
void UAIExtensionSettings::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    if (PropertyChangedEvent.Property != nullptr)
    {
        //const FName PropertyName(PropertyChangedEvent.Property->GetFName());
        
        /*if (PropertyName == GET_MEMBER_NAME_CHECKED(UAIExtensionSettings, Factions))
        {}*/

        auto* Link = PropertyChangedEvent.PropertyChain.GetActiveMemberNode();
        const auto* LinkValue = Link->GetValue();

        if (LinkValue && LinkValue->GetFName() == GET_MEMBER_NAME_CHECKED(UAIExtensionSettings, Relations))
        {
            const int32 RelationIndex = PropertyChangedEvent.GetArrayIndex(LinkValue->GetFName().ToString());

            SanitizeRelations(PropertyChangedEvent.ChangeType, RelationIndex);
        }
    }
}

bool UAIExtensionSettings::CanEditChange(const UProperty* InProperty) const
{
    bool bCanEdit = Super::CanEditChange(InProperty);

    FName PropertyName = InProperty ? InProperty->GetFName() : NAME_None;

    if (GET_MEMBER_NAME_CHECKED(FFactionRelation, Attitude) == PropertyName)
    {
        //TODO: Limit attitude depending on Faction
        bCanEdit &= true;
    }

    return bCanEdit;
}

void UAIExtensionSettings::SanitizeRelations(EPropertyChangeType::Type ChangeType, int32 RelationIndex)
{
    if (ChangeType == EPropertyChangeType::Interactive)
        return;

}
#endif //WITH_EDITOR