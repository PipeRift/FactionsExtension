// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "Customizations/FactionPinFactory.h"


TSharedPtr<class SGraphPin> FFactionPinFactory::CreatePin(class UEdGraphPin* InPin) const
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	if (InPin->PinType.PinCategory == K2Schema->PC_Struct &&
		InPin->PinType.PinSubCategoryObject == FFaction::StaticStruct() &&
		!InPin->PinType.bIsReference)
	{
		return SNew(SFactionPin, InPin); //and return our customized pin widget ;).
	}
	return nullptr;
}