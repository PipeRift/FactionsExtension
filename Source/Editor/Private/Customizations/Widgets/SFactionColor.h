// Copyright 2015-2019 Piperift. All Rights Reserved.
#pragma once

#include <Widgets/SCompoundWidget.h>


class SFactionColor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFactionColor) {}
	SLATE_END_ARGS()

	/** Construct function for this widget */
	void Construct(const FArguments& InArgs, TSharedPtr<IPropertyHandle> _Handle);

	~SFactionColor();

private:

	TSharedPtr<IPropertyHandle> ColorProperty;


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

	bool IsValueEnabled() const;

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
};
