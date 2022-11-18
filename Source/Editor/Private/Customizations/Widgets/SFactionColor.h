// Copyright 2015-2020 Piperift. All Rights Reserved.
#pragma once

#include <Widgets/SCompoundWidget.h>


class SFactionColor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFactionColor) {}
	SLATE_END_ARGS()

	/** Construct function for this widget */
	void Construct(const FArguments& InArgs, TSharedPtr<IPropertyHandle> _Handle);

protected:

	/** Creates the color widget that when clicked spawns the color picker window. */
	TSharedRef<SWidget> CreateColorWidget(TWeakPtr<IPropertyHandle>);

	/**
	 * Get the color used by this struct as a linear color value
	 * @param InColor To be filled with the color value used by this struct, or white if this struct is being used to edit multiple values
	 * @return The result of trying to get the color value
	 */
	FPropertyAccess::Result GetColorAsLinear(FLinearColor& InColor) const;

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
	 * Called to reset all colors to before the color picker spawned
	 */
	void ResetColors();

	/**
	* Called when the user clicks cancel in the color picker
	* The values are reset to their original state when this happens
	*
	* @param OriginalColor Original color of the property
	*/
	void OnColorPickerCancelled(FLinearColor OriginalColor);

	/**
	 * Called when the color picker window is clsoed
	 */
	void OnColorPickerWindowClosed(const TSharedRef<SWindow>& Window);

	/**
	* Called when the user enters an interactive color change (dragging something in the picker)
	*/
	void OnColorPickerInteractiveBegin();

	/**
	* Called when the user completes an interactive color change (dragging something in the picker)
	*/
	void OnColorPickerInteractiveEnd();

	/**
	 * @return The color that should be displayed in the color block
	 */
	FLinearColor OnGetColorForColorBlock() const;

	/**
	 * @return The color that should be displayed in the color block in slate color format
	 */
	FSlateColor  OnGetSlateColorForBlock() const;

	/**
	 * @return The border color encompassing the entire color block
	 */
	FSlateColor GetColorWidgetBorderColor() const;
	/**
	 * Called when the user clicks in the color block (opens inline color picker)
	 */
	FReply OnMouseButtonDownColorBlock(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	/** Called when the user clicks on the the button to get the full color picker */
	FReply OnOpenFullColorPickerClicked();

	/**
	* Called to see if the value is enabled for editing
	*
	* @param WeakHandlePtr	Handle to the property that the new value is for
	*/
	bool IsValueEnabled(TWeakPtr<IPropertyHandle> WeakHandlePtr) const;


protected:
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

	/** Color struct handle */
	TSharedPtr<IPropertyHandle> StructPropertyHandle;

	/** Cached widget for the color picker to use as a parent */
	TSharedPtr<SWidget> ColorPickerParentWidget;

	TSharedPtr<SWidget > ColorWidgetBackgroundBorder;

	/** Overrides the default state of the sRGB check box */
	TOptional<bool> sRGBOverride;

	/** True if the property is a linear color property */
	bool bIsLinearColor = false;

	/** True if the property wants to ignore the alpha component */
	bool bIgnoreAlpha = false;

	/** True if the inline color picker is visible */
	bool bIsInlineColorPickerVisible = false;

	/** True if the user is performing an interactive color change */
	bool bIsInteractive = false;

	/** Last color set from color picker as string*/
	FString LastPickerColorString;


	/** The value won;t be updated while editing */
	bool bDontUpdateWhileEditing = false;
};
