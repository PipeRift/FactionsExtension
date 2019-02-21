// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "Customizations/SFaction.h"

#include "FactionsSettings.h"
#include "PropertyEditorModule.h"


void SFaction::Construct(const FArguments& InArgs, TSharedRef<IPropertyHandle> _FactionHandle)
{
	OnChange = InArgs._OnChange;
	FactionHandle = _FactionHandle;
	if(FactionHandle.IsValid())
		NameHandle = FactionHandle->GetChildHandle("Name");

	ChildSlot
	[
		SAssignNew(ComboBox, SComboBox<TSharedPtr<FName>>)
		.OptionsSource(&CachedItems)
		.OnGenerateWidget(this, &SFaction::HandleStringEnumComboBoxGenerateWidget)
		.OnComboBoxOpening(this, &SFaction::UpdateItems, false)
		.OnSelectionChanged(this, &SFaction::OnSelectionChanged)
		.ButtonStyle(FEditorStyle::Get(), "FlatButton")
		.ForegroundColor(this, &SFaction::GetForegroundColor)
		//.InitiallySelectedItem(GetVariableFactionValue())
		[
			SNew(STextBlock)
			.Text(this, &SFaction::GetSelectedItem)
		]
	];
}

TSharedRef<SWidget> SFaction::HandleStringEnumComboBoxGenerateWidget(const TSharedPtr<FName> Item)
{
	return SNew(STextBlock)
		.Text(FText::FromName(*Item));
}

void SFaction::UpdateItems(bool bRefreshComboBox /*= false*/)
{
	TArray<FName> Names;
	GetFactionNames(Names);

	CachedItems.Empty();

	//Convert FString to Shared Ptrs and Populate the array
	for (auto It = Names.CreateConstIterator(); It; ++It)
	{
		if (It)
		{
			CachedItems.Add(MakeShared<FName>(*It));
		}
	}

	if (bRefreshComboBox && ComboBox.IsValid()) {
		ComboBox->RefreshOptions();
	}
}

void SFaction::OnSelectionChanged(const TSharedPtr<FName> SelectedNamePtr, ESelectInfo::Type SelectInfo)
{
	if (!NameHandle.IsValid())
		return;

	if (SelectedNamePtr.IsValid())
	{
		FName SelectedName = *SelectedNamePtr;
		const auto& AllFactions = GetDefault<UFactionsSettings>()->GetFactionInfos();

		if (SelectedName != NO_FACTION_NAME && AllFactions.Contains(SelectedName))
		{
			NameHandle->SetValue(SelectedName);
		}
		else
		{
			//Priority not found. Set default value
			NameHandle->SetValue(NO_FACTION_NAME);
		}
	}

	UpdateItems(true);
}

FText SFaction::GetSelectedItem() const
{
	FName Id = GetIdValue();
	if (!Id.IsNone())
	{
		return FText::FromName(Id);
	}
	return FText::FromName(NO_FACTION_NAME);
}

void SFaction::GetFactionNames(TArray<FName>& Names) const
{
	const UFactionsSettings* Settings = GetDefault<UFactionsSettings>();
	check(Settings);

	for (const auto& KeyValue : Settings->GetFactionInfos())
	{
		Names.Add(KeyValue.Key);
	}
	// Make sure None is at the start
	Names.Remove(NO_FACTION_NAME);
	Names.Insert(NO_FACTION_NAME, 0);
}

FSlateColor SFaction::GetForegroundColor() const
{
	FName Id = GetIdValue();

	if (Id.IsNone() || GetDefault<UFactionsSettings>()->GetFactionInfos().Contains(Id))
	{
		return FSlateColor::UseForeground();
	}

	return FLinearColor::Red;
}

FName SFaction::GetIdValue() const
{
	FName Id;
	if (NameHandle.IsValid() && NameHandle->GetValue(Id) == FPropertyAccess::Success)
		return Id;
	return FName{};
}
