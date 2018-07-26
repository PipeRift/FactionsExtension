// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "FactionInfoCustomization.h"

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

	HeaderRow
	.NameContent()
	[
		SNew(STextBlock)
		.Font(FEditorStyle::GetFontStyle({ "PropertyWindow.NormalFont" }))
		.ColorAndOpacity(FColor::White)
		.Text(this, &FFactionInfoCustomization::GetTextFromHandle, NameHandle.ToSharedRef())
	];
}

void FFactionInfoCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> DisplayNameHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionInfo, DisplayName));
	TSharedPtr<IPropertyHandle> ColorHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionInfo, Color));
	TSharedPtr<IPropertyHandle> ItselfHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionInfo, AttitudeTowardsItself));
	TSharedPtr<IPropertyHandle> OthersHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionInfo, DefaultAttitudeTowardsOthers));

	StructBuilder.AddProperty(DisplayNameHandle.ToSharedRef());
	StructBuilder.AddProperty(ColorHandle.ToSharedRef());
	StructBuilder.AddProperty(ItselfHandle.ToSharedRef());
	StructBuilder.AddProperty(OthersHandle.ToSharedRef());
}

FText FFactionInfoCustomization::GetTextFromHandle(TSharedRef<IPropertyHandle> Handle) const
{
	FName Text;
	Handle->GetValue(Text);
	return FText::FromName(Text);
}

#undef LOCTEXT_NAMESPACE
