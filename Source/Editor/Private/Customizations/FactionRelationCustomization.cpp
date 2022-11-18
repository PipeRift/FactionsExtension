// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "Customizations/FactionRelationCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailPropertyRow.h"
#include "IDetailChildrenBuilder.h"
#include "Widgets/Views/SListView.h"

#include "FactionsSubsystem.h"
#include "Customizations/SFaction.h"

#define LOCTEXT_NAMESPACE "FFactionRelationCustomization"

TSharedRef<IPropertyTypeCustomization> FFactionRelationCustomization::MakeInstance()
{
	return MakeShareable(new FFactionRelationCustomization);
}

void FFactionRelationCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> AHandle        = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, FactionA));
	TSharedPtr<IPropertyHandle> BHandle        = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, FactionB));
	TSharedPtr<IPropertyHandle> AttitudeHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, Attitude));

	HeaderRow
	.NameContent()
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		[
			CreateFactionWidget(AHandle.ToSharedRef())
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		[
			CreateFactionWidget(BHandle.ToSharedRef())
		]
	]
	.ValueContent()
	.MinDesiredWidth(150.f)
	[
		AttitudeHandle->CreatePropertyValueWidget()
	];
}

void FFactionRelationCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{}

TSharedRef<SWidget> FFactionRelationCustomization::CreateFactionWidget(TSharedRef<IPropertyHandle> PropertyHandle)
{
	auto IdHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFaction, Id));

	return SNew(SFaction)
	.Faction_Lambda([IdHandle]() {
		FName Id;
		IdHandle->GetValue(Id);
		return FFaction{Id};
	})
	.OnFactionSelected_Lambda([IdHandle](FFaction Faction, ESelectInfo::Type) {
		IdHandle->SetValue(Faction.GetId());
	});
}

#undef LOCTEXT_NAMESPACE
