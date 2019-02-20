// Copyright 2015-2019 Piperift. All Rights Reserved.
#pragma once

#include <IPropertyTypeCustomization.h>
#include <Widgets/Input/SComboBox.h>


class FStringEnumCustomization : public IPropertyTypeCustomization
{
public:

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

protected:
	/** The row combobox object */
	TSharedPtr<SComboButton> ComboButton;
	TSharedPtr<SSearchBox> SearchBox;
	TSharedPtr<SListView<TSharedPtr<FString> > > ComboListView;

	/** A cached copy of strings to populate the column combo box */
	TSharedPtr<FString> SelectedItem;
	TArray<TSharedPtr<FString>> CachedItems;

	FString FilterText;


	/** Returns the ListView for the ComboButton */
	TSharedRef<SWidget> GetListContent();

	/** Return the representation of the the row names to display */
	TSharedRef<ITableRow> HandleComboBoxGenerateWidget(TSharedPtr<FString> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	/** Update the root data on a change of selection */
	void OnSelectionChanged(TSharedPtr<FString> NewSelectedItem, ESelectInfo::Type SelectInfo);

	/** Called by Slate when the filter box changes text. */
	void OnFilterTextChanged(const FText& InFilterText);

	void HandleMenuOpen();

	void UpdateItems(bool bRefresh = true);


	/** Called at customization's setup
	 * Default: false
	 */
	virtual bool CanCustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
	{ return false; }



	/** Returns all the enum items.
	* This is for override purposes.
	*/
	virtual void GetAllItems(TArray<FString>& Values) const;

	/** Returns the actually selected item.
	* This is for override purposes.
	*/
	FString GetSelectedItem() const {
		return SelectedItem.IsValid()? *SelectedItem : TEXT("");
	};

	/** Returns the actually selected item as text.
	* This is for override purposes.
	*/
	virtual FText GetSelectedText() const;


	/** Called when selection changed.
	* This is for override purposes.
	*/
	virtual void OnItemSelected(FString Value) {}

	virtual FSlateColor GetForegroundColor() const;
};

