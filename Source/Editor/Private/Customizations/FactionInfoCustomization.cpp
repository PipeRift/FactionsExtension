// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "Customizations/FactionInfoCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "Widgets/Views/SListView.h"

#include "FactionsSettings.h"

#define LOCTEXT_NAMESPACE "FFactionInfoCustomization"

TSharedRef<IPropertyTypeCustomization> FFactionInfoCustomization::MakeInstance()
{
	return MakeShareable(new FFactionInfoCustomization);
}

void FFactionInfoCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> NameHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionInfo, DisplayName));

	HeaderRow.NameContent()
	[
		StructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		NameHandle->CreatePropertyValueWidget()
	];
}

void FFactionInfoCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> NameHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionInfo, DisplayName));

	uint32 Num = 0;
	StructPropertyHandle->GetNumChildren(Num);

	for (uint32 I = 0; I < Num; ++I)
	{
		TSharedPtr<IPropertyHandle> PropHandle = StructPropertyHandle->GetChildHandle(I);
		check(PropHandle);

		if (PropHandle->GetProperty() != NameHandle->GetProperty())
		{
			StructBuilder.AddProperty(PropHandle.ToSharedRef());
		}
	}
}

#undef LOCTEXT_NAMESPACE
