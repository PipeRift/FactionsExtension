// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "Layout/Visibility.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Input/Reply.h"
#include "PropertyHandle.h"
#include "EditorUndoClient.h"

class IDetailsView;
class IDetailLayoutBuilder;
class UFactionsSettings;


struct FFactionInfoMapItem {
	FFactionInfoMapItem() : Id{ INDEX_NONE }, Name{}, MapProperty{} {}
	FFactionInfoMapItem(int32 Id, FName Name, const TSharedPtr<IPropertyHandle>& Property)
		: Id{Id}, Name{Name}, MapProperty{Property}
	{}

	int32 Id;
	FName Name;
	TSharedPtr<IPropertyHandle> MapProperty;

	bool IsValid() const { return Id != INDEX_NONE && MapProperty.IsValid(); }

	TSharedPtr<IPropertyHandle> GetProperty() const {
		return MapProperty.IsValid() ? MapProperty->GetChildHandle(Id) : nullptr;
	}
};


class FFactionsSettingsDetails : public IDetailCustomization, public FEditorUndoClient
{
	static const FName NewFactionName;

public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	//~ Begin FEditorUndoClient Interface
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	//~ End FEditorUndoClient Interface

	virtual ~FFactionsSettingsDetails();

private:

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void CustomizeFactionsDetails(IDetailLayoutBuilder& DetailBuilder);

	/** Make the widget for a row entry in the data table row list view */
	TSharedRef<ITableRow> MakeFactionWidget(const TSharedPtr<FFactionInfoMapItem> FactionName, const TSharedRef<STableViewBase>& OwnerTable);

	void SetFactionSelection(TSharedPtr<FFactionInfoMapItem> InNewSelection, ESelectInfo::Type InSelectInfo = ESelectInfo::Direct);

	void RefreshFactions();

	void OnFactionFilterChanged(const FText& Text);

	FReply OnNewFaction();
	FReply OnClearFactions();

public:

	void OnFactionIdChange(const FText& NewId, ETextCommit::Type CommitInfo, const TSharedPtr<FFactionInfoMapItem> Faction);
	FReply OnDeleteFaction(const TSharedPtr<FFactionInfoMapItem> Faction);


private:

	TWeakObjectPtr<UFactionsSettings> Settings;
	TSharedPtr<IPropertyHandle> FactionsMapHandle;


	/** Array of the relations that are available for editing */
	TArray<TSharedPtr<FFactionInfoMapItem>> AvailableFactions;

	/** Array of the relations that are available for editing */
	TArray<TSharedPtr<FFactionInfoMapItem>> VisibleFactions;

	/** List view responsible for showing the rows in VisibleRows for each entry in Factions */
	TSharedPtr<SListView<TSharedPtr<FFactionInfoMapItem>>> FactionsListView;

	TSharedPtr<SVerticalBox> FactionContainer;

	TWeakPtr<FFactionInfoMapItem> CurrentSelection;

	FString SearchFilter;

public:

	static const FName ColumnSelect;
	static const FName ColumnId;
	static const FName ColumnColor;
	static const FName ColumnDelete;
};


class SFactionViewItem : public SMultiColumnTableRow<TSharedPtr<FFactionInfoMapItem>>
{
public:
	SLATE_BEGIN_ARGS(SFactionViewItem) {}
		/** The widget that owns the tree.  We'll only keep a weak reference to it. */
		SLATE_ARGUMENT(TSharedPtr<FFactionsSettingsDetails>, Details)
		SLATE_ARGUMENT(TSharedPtr<FFactionInfoMapItem>, Faction)
	SLATE_END_ARGS()

	/** Construct function for this widget */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

	/** Overridden from SMultiColumnTableRow.  Generates a widget for this column of the list view. */
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& Column) override;

private:

	/** Begin Name */
	FText GetEditableNameAsText() const { return FText::FromName(Faction->Name); }
	FReply OnIdNameDoubleClicked();
	void OnIdNameCommited(const FText& InText, ETextCommit::Type InCommitType) const;
	/** End Name */

	/** Begin Color */

	/** Creates the color widget that when clicked spawns the color picker window. */
	TSharedRef<SWidget> CreateColorWidget(TWeakPtr<IPropertyHandle> Property);

	/**
	* Creates a new color picker for interactively selecting the color
	*
	* @param bUseAlpha If true alpha will be displayed, otherwise it is ignored
	*/
	void CreateColorPicker(bool bUseAlpha);

	/**
	* Called when the property is set from the color picker
	*
	* @param NewColor The new color value
	*/
	void OnSetColorFromColorPicker(FLinearColor NewColor);

	/**
	* Called when the user clicks cancel in the color picker
	* The values are reset to their original state when this happens
	*
	* @param OriginalColor Original color of the property
	*/
	void OnColorPickerCancelled(FLinearColor OriginalColor);

	/**
	* Called when the user enters an interactive color change (dragging something in the picker)
	*/
	void OnColorPickerInteractiveBegin();

	/**
	* Called when the user completes an interactive color change (dragging something in the picker)
	*/
	void OnColorPickerInteractiveEnd();

	/**
	* Get the color used by this struct as a linear color value
	* @param InColor To be filled with the color value used by this struct, or white if this struct is being used to edit multiple values
	* @return The result of trying to get the color value
	*/
	FPropertyAccess::Result GetColorAsLinear(FLinearColor& OutColor) const;

	/**  @return The color that should be displayed in the color block */
	FLinearColor OnGetColorForColorBlock() const;

	/** Called when the user clicks in the color block (opens inline color picker) */
	FReply OnMouseButtonDownColorBlock(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	/** End Color */

	/**
	* Called to see if the value is enabled for editing
	*
	* @param WeakHandlePtr	Handle to the property that the new value is for
	*/
	bool IsValueEnabled(TWeakPtr<IPropertyHandle> WeakHandlePtr) const;


	/** Weak reference to the database customization that owns our list */
	TWeakPtr<FFactionsSettingsDetails> Details;

	TSharedPtr<FFactionInfoMapItem> Faction;


	TSharedPtr<IPropertyHandle> InfoColorProperty;

	/** Stores a linear or srgb color without converting between the two. Only one is valid at a time */
	struct FLinearOrSrgbColor
	{
		FLinearOrSrgbColor(const FLinearColor& InLinearColor)
			: LinearColor(InLinearColor)
		{}

		FLinearOrSrgbColor(const FColor& InSrgbColor)
			: SrgbColor(InSrgbColor)
		{}

		FLinearColor GetLinear() const { return LinearColor; }
		FColor GetSrgb() const { return SrgbColor; }
	private:
		FLinearColor LinearColor;
		FColor SrgbColor;
	};

	/** Saved per struct colors in case the user clicks cancel in the color picker */
	TArray<FLinearOrSrgbColor> SavedPreColorPickerColors;

	bool bDontUpdateWhileEditingColor;

	/** True if the property is a linear color property */
	bool bColorIsLinear;

	/** True if the property wants to ignore the alpha component */
	bool bColorIgnoreAlpha;

	/** True if the user is performing an interactive color change */
	bool bColorIsInteractive;

	/** Cached widget for the color picker to use as a parent */
	TSharedPtr<SWidget> ColorPickerParentWidget;


	TSharedPtr<SWidgetSwitcher> IdNameSwitcher;
	TSharedPtr<SEditableTextBox> IdNameTextBox;
};
