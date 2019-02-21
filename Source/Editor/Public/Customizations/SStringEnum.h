// Copyright 2015-2019 Piperift. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <PropertyHandle.h>
#include <Widgets/SCompoundWidget.h>
#include <Widgets/Input/SComboBox.h>


class SStringEnum : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SStringEnum)
		: _OnChange()
	{}

	SLATE_EVENT(FSimpleDelegate, OnChange)
	SLATE_END_ARGS()

	/**
	 * Construct the faction widget from a declaration
	 *
	 * @param InArgs    Widget declaration from which to construct the faction.
	 */
	void Construct(const FArguments& InArgs, TSharedRef<IPropertyHandle> _Handle);

private:


	/** Return the representation of the the column names to display */
	TSharedRef<SWidget> HandleStringEnumComboBoxGenerateWidget(const TSharedPtr<FName> Item);

	void UpdateItems(bool bRefreshComboBox = false);

	/** Update the root data on a change of selection */
	void OnSelectionChanged(const TSharedPtr<FName> SelectedName, ESelectInfo::Type SelectInfo);

	/** Returns the actually selected item.
	 * This is for override purposes.
	 */
	FText GetSelectedItem() const;


	/** Returns all the enum items.
	* This is for override purposes.
	*/
	void GetFactionNames(TArray<FName>& Names) const;


	TSharedPtr<IPropertyHandle> FactionHandle;
	TSharedPtr<IPropertyHandle> NameHandle;

	FSimpleDelegate OnChange;


	/** A cached copy of strings to populate the column combo box */
	TArray<TSharedPtr<FName>> CachedItems;

	TSharedPtr<SComboBox<TSharedPtr<FName>>> ComboBox;
};
