// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "SFactionColor.h"

#include <Editor.h>
#include <ScopedTransaction.h>
#include <PropertyHandle.h>
#include <Layout/WidgetPath.h>
#include <Framework/Application/SlateApplication.h>
#include <Widgets/Input/SButton.h>
#include <Widgets/Colors/SColorBlock.h>
#include <Widgets/Colors/SColorPicker.h>


#define LOCTEXT_NAMESPACE "SFactionColor"

bool IsEditable(TSharedPtr<IPropertyHandle> WeakHandlePtr)
{
	if (WeakHandlePtr.IsValid())
	{
		return !WeakHandlePtr->IsEditConst();
	}
	return false;
}


/************************************************************************
* SFactionColor
*/

void SFactionColor::Construct(const FArguments& InArgs, TSharedPtr<IPropertyHandle> _Handle)
{
	ColorProperty = _Handle;

	if (ColorProperty.IsValid() && ColorProperty->IsValidHandle())
	{
		bColorIsLinear = CastFieldChecked<FStructProperty>(ColorProperty->GetProperty())->Struct->GetFName() == NAME_LinearColor;
		bColorIgnoreAlpha = ColorProperty->GetProperty()->HasMetaData(TEXT("HideAlphaChannel"));
	}
	bDontUpdateWhileEditingColor = true;


	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 2.0f)
		[
			// Displays the color with alpha unless it is ignored
			SAssignNew(ColorPickerParentWidget, SColorBlock)
			.Color(this, &SFactionColor::OnGetColorForColorBlock)
			.ShowBackgroundForAlpha(true)
			.IgnoreAlpha(bColorIgnoreAlpha)
			.OnMouseButtonDown(this, &SFactionColor::OnMouseButtonDownColorBlock)
			.Size(FVector2D(35.0f, 12.0f))
			.IsEnabled(this, &SFactionColor::IsValueEnabled)
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 2.0f)
		[
			// Displays the color without alpha
			SNew(SColorBlock)
			.Color(this, &SFactionColor::OnGetColorForColorBlock)
			.ShowBackgroundForAlpha(false)
			.IgnoreAlpha(true)
			.OnMouseButtonDown(this, &SFactionColor::OnMouseButtonDownColorBlock)
			.Size(FVector2D(35.0f, 12.0f))
		]
	];
}

SFactionColor::~SFactionColor()
{
	DestroyColorPicker();
}

void SFactionColor::CreateColorPicker(bool bUseAlpha)
{
	int32 NumObjects = ColorProperty->GetNumOuterObjects();

	SavedPreColorPickerColors.Empty();
	TArray<FString> PerObjectValues;
	ColorProperty->GetPerObjectValues(PerObjectValues);

	for (int32 ObjectIndex = 0; ObjectIndex < NumObjects; ++ObjectIndex)
	{
		if (bColorIsLinear)
		{
			FLinearColor Color;
			Color.InitFromString(PerObjectValues[ObjectIndex]);
			SavedPreColorPickerColors.Add(FLinearOrSrgbColor(Color));
		}
		else
		{
			FColor Color;
			Color.InitFromString(PerObjectValues[ObjectIndex]);
			SavedPreColorPickerColors.Add(FLinearOrSrgbColor(Color));
		}
	}

	FLinearColor InitialColor;
	GetColorAsLinear(InitialColor);

	const bool bRefreshOnlyOnOk = bDontUpdateWhileEditingColor || ColorProperty->HasMetaData("DontUpdateWhileEditing");

	FColorPickerArgs PickerArgs {};
	{
		PickerArgs.bUseAlpha = !bColorIgnoreAlpha;
		PickerArgs.bOnlyRefreshOnMouseUp = false;
		PickerArgs.bOnlyRefreshOnOk = bRefreshOnlyOnOk;
		PickerArgs.sRGBOverride = false;
		PickerArgs.DisplayGamma = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateUObject(GEngine, &UEngine::GetDisplayGamma));
		PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &SFactionColor::OnSetColorFromColorPicker);
		PickerArgs.OnColorPickerCancelled = FOnColorPickerCancelled::CreateSP(this, &SFactionColor::OnColorPickerCancelled);
		PickerArgs.OnInteractivePickBegin = FSimpleDelegate::CreateSP(this, &SFactionColor::OnColorPickerInteractiveBegin);
		PickerArgs.OnInteractivePickEnd = FSimpleDelegate::CreateSP(this, &SFactionColor::OnColorPickerInteractiveEnd);
		PickerArgs.InitialColorOverride = InitialColor;
		PickerArgs.ParentWidget = ColorPickerParentWidget;
		PickerArgs.OptionalOwningDetailsView = ColorPickerParentWidget;
		FWidgetPath ParentWidgetPath;
		if (FSlateApplication::Get().FindPathToWidget(ColorPickerParentWidget.ToSharedRef(), ParentWidgetPath))
		{
			PickerArgs.bOpenAsMenu = FSlateApplication::Get().FindMenuInWidgetPath(ParentWidgetPath).IsValid();
		}
	}

	OpenColorPicker(PickerArgs);
}

void SFactionColor::OnSetColorFromColorPicker(FLinearColor NewColor)
{
	FString ColorString;
	if (bColorIsLinear)
	{
		ColorString = NewColor.ToString();
	}
	else
	{
		const bool bSRGB = true;
		FColor NewFColor = NewColor.ToFColor(bSRGB);
		ColorString = NewFColor.ToString();
	}

	ColorProperty->SetValueFromFormattedString(ColorString, bColorIsInteractive ? EPropertyValueSetFlags::InteractiveChange : 0);
	ColorProperty->NotifyFinishedChangingProperties();
}

void SFactionColor::OnColorPickerCancelled(FLinearColor OriginalColor)
{
	TArray<FString> PerObjectColors;

	for (int32 ColorIndex = 0; ColorIndex < SavedPreColorPickerColors.Num(); ++ColorIndex)
	{
		if (bColorIsLinear)
		{
			PerObjectColors.Add(SavedPreColorPickerColors[ColorIndex].GetLinear().ToString());
		}
		else
		{
			FColor Color = SavedPreColorPickerColors[ColorIndex].GetSrgb();
			PerObjectColors.Add(Color.ToString());
		}
	}

	if (PerObjectColors.Num() > 0)
	{
		ColorProperty->SetPerObjectValues(PerObjectColors);
	}
}

void SFactionColor::OnColorPickerInteractiveBegin()
{
	bColorIsInteractive = true;
	GEditor->BeginTransaction(FText::Format(LOCTEXT("SetColorProperty", "Edit {0}"), ColorProperty->GetPropertyDisplayName()));
}

void SFactionColor::OnColorPickerInteractiveEnd()
{
	bColorIsInteractive = false;

	if (!bDontUpdateWhileEditingColor)
	{
		// pushes the last value from the interactive change without the interactive flag
		FString ColorString;
		ColorProperty->GetValueAsFormattedString(ColorString);
		ColorProperty->SetValueFromFormattedString(ColorString);
	}

	GEditor->EndTransaction();
}

FPropertyAccess::Result SFactionColor::GetColorAsLinear(FLinearColor& OutColor) const
{
	if (!ColorProperty.IsValid())
		return FPropertyAccess::Fail;

	// Default to full alpha in case the alpha component is disabled.
	OutColor.A = 1.0f;

	FString StringValue;
	FPropertyAccess::Result Result = ColorProperty->GetValueAsFormattedString(StringValue);

	if (Result == FPropertyAccess::Success)
	{
		if (bColorIsLinear)
		{
			OutColor.InitFromString(StringValue);
		}
		else
		{
			FColor SrgbColor;
			SrgbColor.InitFromString(StringValue);
			OutColor = FLinearColor(SrgbColor);
		}
	}
	else if (Result == FPropertyAccess::MultipleValues)
	{
		OutColor = FLinearColor::White;
	}

	return Result;
}

FLinearColor SFactionColor::OnGetColorForColorBlock() const
{
	FLinearColor Color;
	GetColorAsLinear(Color);
	return Color;
}

FReply SFactionColor::OnMouseButtonDownColorBlock(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return FReply::Unhandled();
	}

	if (IsEditable(ColorProperty))
	{
		CreateColorPicker(true);
	}

	return FReply::Handled();
}

bool SFactionColor::IsValueEnabled() const
{
	return IsEditable(ColorProperty);
}

#undef LOCTEXT_NAMESPACE
