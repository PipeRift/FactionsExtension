// Copyright 2015-2018 Piperift. All Rights Reserved.
#pragma once

#include "IPropertyTypeCustomization.h"
#include "SComboBox.h"


class FStringEnumCustomization : public IPropertyTypeCustomization
{
public:
    /**
    * Creates a new instance.
    *
    * @return A new struct customization.
    */
    static TSharedRef<IPropertyTypeCustomization> MakeInstance() 
    {
        return MakeShareable(new FStringEnumCustomization);
    }

    /** IPropertyTypeCustomization interface */
    virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
    virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

protected:
    /** The row combobox object */
    TSharedPtr<SComboBox<TSharedPtr<FString>>> ComboBox;

    /** A cached copy of strings to populate the column combo box */
    TArray<TSharedPtr<FString>> CachedItems;

    /** Return the representation of the the column names to display */
    TSharedRef<SWidget> HandleStringEnumComboBoxGenerateWidget(TSharedPtr<FString> Item);

    /** Display the current column selection */
    FText GetStringEnumComboBoxContentText() const;

    /** Update the root data on a change of selection */
    void OnSelectionChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo);

    void UpdateItems(bool bRefresh = true);


    /** Called at customization's setup
     * Default: false
     */
    virtual bool CanCustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
    { return false; }



    /** Returns all the enum items.
    * This is for override purposes.
    */
    virtual void GetEnumItems(TArray<FString>& Values) const;

    /** Called when selection changed.
    * This is for override purposes.
    */
    virtual void OnItemSelected(FString Value){}

    /** Returns the actually selected item.
    * This is for override purposes.
    */
    virtual FText GetSelectedItem() const;
};

