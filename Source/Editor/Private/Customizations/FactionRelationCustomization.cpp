// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "Customizations/FactionRelationCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailPropertyRow.h"
#include "IDetailChildrenBuilder.h"
#include "Widgets/Views/SListView.h"

#include "FactionsSubsystem.h"
#include "Customizations/SFaction.h"
#include "FactionsEditorStyle.h"

#define LOCTEXT_NAMESPACE "FFactionRelationCustomization"

TSharedRef<IPropertyTypeCustomization> FFactionRelationCustomization::MakeInstance()
{
	return MakeShareable(new FFactionRelationCustomization);
}

void FFactionRelationCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> SourceHandle        = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, Source));
	TSharedPtr<IPropertyHandle> TargetHandle        = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, Target));
	TSharedPtr<IPropertyHandle> AttitudeHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, Attitude));
	TSharedPtr<IPropertyHandle> BidirectionalHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionRelation, bBidirectional));

	HeaderRow
	.NameContent()
	.HAlign(HAlign_Fill)
	[
		StructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.HAlign(HAlign_Fill)
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		[
			CreateFactionWidget(SourceHandle.ToSharedRef())
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SCheckBox)
			.Style(FFactionsEditorStyle::Get(), "Relation.DirectionalCheckBox")
			.IsChecked_Lambda([=](){
				bool bValue = false;
				BidirectionalHandle->GetValue(bValue);
				return bValue ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
			})
			.OnCheckStateChanged_Lambda([=](const ECheckBoxState NewState){
				BidirectionalHandle->SetValue(NewState == ECheckBoxState::Checked);
			})
			.ToolTipText(BidirectionalHandle->GetToolTipText())
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		[
			CreateFactionWidget(TargetHandle.ToSharedRef())
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		[
			AttitudeHandle->CreatePropertyValueWidget()
		]
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
