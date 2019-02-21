// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "Customizations/FactionCustomization.h"

#include "FactionsModule.h"

#include "Faction.h"
#include "FactionsSettings.h"

#define LOCTEXT_NAMESPACE "FFactionCustomization"


bool FFactionCustomization::CanCustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;
	NameHandle = StructPropertyHandle->GetChildHandle("Name");

	if (NameHandle->IsValidHandle())
	{
		FFactionsModule& Module = FFactionsModule::Get();
		//Bind On Settings Changed event
		Module.OnModifiedSettings().BindRaw(this, &FFactionCustomization::UpdateItems, true);
		return true;
	}
	return false;
}

void FFactionCustomization::GetAllItems(TArray<FString>& Values) const
{
	for (const auto& KeyValue : GetDefault<UFactionsSettings>()->GetFactionInfos())
	{
		Values.Add(KeyValue.Key.ToString());
	}
	// Make sure None is at the start
	Values.Remove(NO_FACTION_NAME.ToString());
	Values.Insert(NO_FACTION_NAME.ToString(), 0);
}

void FFactionCustomization::OnItemSelected(FString Value)
{
	const auto& Factions = GetDefault<UFactionsSettings>()->GetFactionInfos();

	FName NameValue = FName(*Value);

	if (NameValue != NO_FACTION_NAME && Factions.Contains(NameValue))
	{
		NameHandle->SetValue(NameValue);
	}
	else
	{
		//Priority not found. Set default value
		NameHandle->SetValue(NO_FACTION_NAME);
	}
}

/** Display the current column selection */
FText FFactionCustomization::GetSelectedText() const
{
	FName Id = GetIdValue();
	if (!Id.IsNone())
	{
		return FText::FromName(Id);
	}
	return FText::FromName(NO_FACTION_NAME);
}

FSlateColor FFactionCustomization::GetForegroundColor() const
{
	FName Id = GetIdValue();

	if (Id.IsNone() || GetDefault<UFactionsSettings>()->GetFactionInfos().Contains(Id))
	{
		return FStringEnumCustomization::GetForegroundColor();
	}

	return FLinearColor::Red;
}

FName FFactionCustomization::GetIdValue() const
{
	FName Id;
	if(NameHandle.IsValid() && NameHandle->GetValue(Id) == FPropertyAccess::Success)
		return Id;
	return FName{};
}

#undef LOCTEXT_NAMESPACE
