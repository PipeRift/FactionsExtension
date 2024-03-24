// Copyright 2015-2023 Piperift. All Rights Reserved.

#include "Customizations/SFaction.h"

#include <FactionsSubsystem.h>
#include <PropertyEditorModule.h>


SFaction::~SFaction()
{
	FFactionsModule& Module = FFactionsModule::Get();
	Module.OnModifiedSettings().Remove(OnModifiedSettingsHandle);
}

void SFaction::Construct(const FArguments& InArgs)
{
	Faction = InArgs._Faction;

	OnFactionSelected = InArgs._OnFactionSelected;

	UpdateItems();

	// Bind On Settings changed event
	FFactionsModule& Module = FFactionsModule::Get();
	OnModifiedSettingsHandle =
		Module.OnModifiedSettings().Add(FSimpleDelegate::CreateRaw(this, &SFaction::UpdateItems, true));

	ChildSlot[SAssignNew(ComboBox, SSearchableComboBox)
				  .OptionsSource(&CachedItems)
				  .OnGenerateWidget_Lambda([](TSharedPtr<FString> Item) {
					  return SNew(STextBlock).Text(FText::FromString(*Item));
				  })
				  .OnComboBoxOpening(this, &SFaction::UpdateItems, false)
				  .OnSelectionChanged(this, &SFaction::OnSelectionChanged)
				  .ForegroundColor(this, &SFaction::GetForegroundColor)
				  .HasDownArrow(InArgs._HasDownArrow)
				  .ContentPadding(InArgs._ContentPadding)
				  .ComboBoxStyle(InArgs._ComboBoxStyle)
				  .ButtonStyle(InArgs._ButtonStyle)
				  .ItemStyle(InArgs._ItemStyle)[SNew(STextBlock).Text(this, &SFaction::GetSelectedItem)]];
}

void SFaction::UpdateItems(bool bRefreshComboBox /*= false*/)
{
	TArray<FString> Names;
	GetFactionNames(Names);

	CachedItems.Empty();

	// Convert FString to Shared Ptrs and Populate the array
	for (auto It = Names.CreateIterator(); It; ++It)
	{
		if (It)
		{
			CachedItems.Add(MakeShared<FString>(MoveTemp(*It)));
		}
	}

	if (bRefreshComboBox && ComboBox.IsValid())
	{
		ComboBox->RefreshOptions();
	}
}

void SFaction::OnSelectionChanged(const TSharedPtr<FString> Selected, ESelectInfo::Type SelectInfo)
{
	if (Selected.IsValid())
	{
		FFaction NewFaction{FName{*Selected}};
		OnFactionSelected.ExecuteIfBound(NewFaction, SelectInfo);
	}

	UpdateItems(true);
}

FText SFaction::GetSelectedItem() const
{
	FFaction Id = GetFaction();
	if (!Id.IsNone())
	{
		return FText::FromName(Id.GetId());
	}
	return FText::FromName(FFaction::NoId);
}

void SFaction::GetFactionNames(TArray<FString>& Names) const
{
	const UFactionsSubsystem* Settings = GetDefault<UFactionsSubsystem>();
	check(Settings);

	for (const auto& It : Settings->GetFactions().List)
	{
		if (It.Key != FFaction::NoId)
		{
			Names.Add(It.Key.ToString());
		}
	}

	// Make sure None is at the start
	Names.Insert(FFaction::NoId.ToString(), 0);
}

FSlateColor SFaction::GetForegroundColor() const
{
	FFaction Id = GetFaction();
	if (Id.IsNone() || GetDefault<UFactionsSubsystem>()->IsValid(Id))
	{
		return FSlateColor::UseForeground();
	}

	return FLinearColor::Red;
}

FFaction SFaction::GetFaction() const
{
	return Faction.Get();
}
