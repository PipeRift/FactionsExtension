// Copyright 2015-2019 Piperift. All Rights Reserved.

#pragma once

#include "SGraphPin.h"
#include "Widgets/Input/SComboBox.h"


class SStringEnumPin : public SGraphPin
{
public:
    SLATE_BEGIN_ARGS(SStringEnumPin) {}
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

    //Display slate widget used for customization.
    virtual TSharedRef<SWidget> GetDefaultValueWidget() override;

protected:
    /** The row combobox object */
    TSharedPtr<SComboBox<TSharedPtr<FString>>> ComboBox;

    /** A cached copy of strings to populate the column combo box */
    TArray<TSharedPtr<FString>> CachedItems;

    /** Return the representation of the the column names to display */
    TSharedRef<SWidget> HandleStringEnumComboBoxGenerateWidget(TSharedPtr<FString> Item);

    /** Update the root data on a change of selection */
    void OnSelectionChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo);

    void UpdateItems(bool bRefresh = true);



    /** Returns all the enum items.
    * This is for override purposes.
    */
    virtual void GetEnumItems(TArray<FString>& Values);

    /** Called when selection changed.
    * This is for override purposes.
    */
    virtual void OnItemSelected(FString Value) {}

    /** Returns the actually selected item.
    * This is for override purposes.
    */
    virtual FText GetSelectedItem() const;
};