// Copyright 2015-2020 Piperift. All Rights Reserved.
#pragma once

#include <IPropertyTypeCustomization.h>
#include <PropertyHandle.h>


class FFactionCustomization : public IPropertyTypeCustomization
{
public:
	/**
	 * Creates a new instance.
	 *
	 * @return A new struct customization for Factions.
	 */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShared<FFactionCustomization>();
	}

protected:
	/** Handle to the struct properties being customized */
	TSharedPtr<IPropertyHandle> StructHandle;


	void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle,
		class FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
		IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override
	{}
};
