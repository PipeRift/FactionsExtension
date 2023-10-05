// Copyright 2015-2023 Piperift. All Rights Reserved.

#include "Customizations/FactionCustomization.h"

#include "Customizations/SFaction.h"
#include "Faction.h"
#include "FactionsModule.h"
#include "FactionsSubsystem.h"

#include <DetailWidgetRow.h>



#define LOCTEXT_NAMESPACE "FFactionCustomization"


void FFactionCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle,
	class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;
	if (!StructHandle->IsValidHandle())
	{
		return;
	}

	auto IdHandle = StructHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFaction, Id));

	HeaderRow.NameContent()[StructHandle->CreatePropertyNameWidget()]
		.ValueContent()
		.MinDesiredWidth(150.0f)
		.MaxDesiredWidth(
			250.0f)[SNew(SFaction)
						.Faction_Lambda([IdHandle]() {
							FName Id;
							IdHandle->GetValue(Id);
							return FFaction{Id};
						})
						.OnFactionSelected_Lambda([IdHandle](FFaction Faction, ESelectInfo::Type) {
							IdHandle->SetValue(Faction.GetId());
						})];
}

#undef LOCTEXT_NAMESPACE
