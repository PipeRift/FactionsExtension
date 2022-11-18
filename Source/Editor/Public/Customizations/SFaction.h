// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <Widgets/DeclarativeSyntaxSupport.h>
#include <Widgets/SCompoundWidget.h>
#include <PropertyHandle.h>
#include <SSearchableComboBox.h>


class SFaction : public SCompoundWidget
{
public:
	using FOnFactionSelected = typename TSlateDelegates<FFaction>::FOnSelectionChanged;

	SLATE_BEGIN_ARGS(SFaction)
		:  _ComboBoxStyle(&FAppStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox"))
		, _ButtonStyle(nullptr)
		, _ItemStyle(&FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("ComboBox.Row"))
		, _ContentPadding(_ComboBoxStyle->ContentPadding)
		, _HasDownArrow(true)
		, _Faction()
		, _OnFactionSelected()
	{}
	SLATE_STYLE_ARGUMENT(FComboBoxStyle, ComboBoxStyle)
	SLATE_STYLE_ARGUMENT(FButtonStyle, ButtonStyle)
	SLATE_STYLE_ARGUMENT(FTableRowStyle, ItemStyle)
	SLATE_ATTRIBUTE(FMargin, ContentPadding)
	SLATE_ARGUMENT(bool, HasDownArrow)
	SLATE_ATTRIBUTE(FFaction, Faction)
	SLATE_EVENT(FOnFactionSelected, OnFactionSelected)
	SLATE_END_ARGS()

	~SFaction();

	/**
	 * Construct the faction widget from a declaration
	 *
	 * @param InArgs    Widget declaration from which to construct the faction.
	 */
	void Construct(const FArguments& InArgs);

private:
	void UpdateItems(bool bRefreshComboBox = false);

	/** Update the root data on a change of selection */
	void OnSelectionChanged(const TSharedPtr<FString> Selected, ESelectInfo::Type SelectInfo);

	/** Returns the actually selected item.
	 * This is for override purposes.
	 */
	FText GetSelectedItem() const;


	/** Returns all the enum items.
	 * This is for override purposes.
	 */
	void GetFactionNames(TArray<FString>& Names) const;

	FSlateColor GetForegroundColor() const;

	FFaction GetFaction() const;

	TAttribute<FFaction> Faction;

	FOnFactionSelected OnFactionSelected;

	FDelegateHandle OnModifiedSettingsHandle;


	/** A cached copy of strings to populate the column combo box */
	TArray<TSharedPtr<FString>> CachedItems;

	TSharedPtr<SSearchableComboBox> ComboBox;
};
