#pragma once

#include <SGraphPin.h>


class SFactionPin : public SGraphPin
{
public:
    SLATE_BEGIN_ARGS(SFactionPin) {}
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

    //Display slate widget used for customization.
    virtual TSharedRef<SWidget> GetDefaultValueWidget() override;

private:
    void ParseDefaultValue();
    void ApplyDefaultValue();


protected:
    FName FactionDefaultNameValue;
};