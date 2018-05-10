// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "FactionInfo.h"

#include "FactionsSettings.h"


/*void FFactionInfo::SetRelation(const FFaction& OtherFaction, const ETeamAttitude::Type Attitude)
{
    if (OtherFaction.IsNone())
        return;

    FFactionRelation InRelation(this->GetFaction(), OtherFaction, Attitude);

    TArray<FFactionRelation>& Relations = GetMutableDefault<UFactionsSettings>()->Relations;

    //Remove possible similar relation
    FFactionRelation* const FoundRelationPtr = Relations.FindByKey(InRelation);
    if (FoundRelationPtr == NULL) 
    {
        Relations.AddUnique(InRelation);
    }
    else
    {
        FoundRelationPtr->Attitude = Attitude;
    }
}*/
