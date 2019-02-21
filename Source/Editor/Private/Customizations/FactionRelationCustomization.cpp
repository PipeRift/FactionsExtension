// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "Customizations/FactionRelationCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailPropertyRow.h"
#include "IDetailChildrenBuilder.h"
#include "Widgets/Views/SListView.h"

#include "FactionsSettings.h"
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
	return SNew(SFaction, PropertyHandle);
}

#undef LOCTEXT_NAMESPACE
