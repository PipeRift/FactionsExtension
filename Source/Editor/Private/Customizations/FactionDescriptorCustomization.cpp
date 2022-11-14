// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "Customizations/FactionDescriptorCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "Widgets/Views/SListView.h"

#include "FactionsSettings.h"

#define LOCTEXT_NAMESPACE "FFactionDescriptorCustomization"

TSharedRef<IPropertyTypeCustomization> FFactionDescriptorCustomization::MakeInstance()
{
	return MakeShareable(new FFactionDescriptorCustomization);
}

void FFactionDescriptorCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> DisplayNameHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionDescriptor, DisplayName));

	HeaderRow.NameContent()
	[
		StructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		DisplayNameHandle->CreatePropertyValueWidget()
	];
}

void FFactionDescriptorCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> DisplayNameHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionDescriptor, DisplayName));

	uint32 Num = 0;
	StructPropertyHandle->GetNumChildren(Num);

	for (uint32 I = 0; I < Num; ++I)
	{
		TSharedPtr<IPropertyHandle> PropHandle = StructPropertyHandle->GetChildHandle(I);
		check(PropHandle);

		if (PropHandle->GetProperty() != DisplayNameHandle->GetProperty())
		{
			StructBuilder.AddProperty(PropHandle.ToSharedRef());
		}
	}
}

#undef LOCTEXT_NAMESPACE
