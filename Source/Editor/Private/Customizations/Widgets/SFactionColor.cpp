// Copyright 2015-2020 Piperift. All Rights Reserved.

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

void SFactionColor::Construct(const FArguments& InArgs, TSharedPtr<IPropertyHandle> _Handle)
{
	StructPropertyHandle = _Handle;

	bIsLinearColor = CastFieldChecked<FStructProperty>(StructPropertyHandle->GetProperty())->Struct->GetFName() == NAME_LinearColor;
	bIgnoreAlpha = StructPropertyHandle->GetProperty()->HasMetaData(TEXT("HideAlphaChannel"));
	
	if (StructPropertyHandle->GetProperty()->HasMetaData(TEXT("sRGB")))
	{
		sRGBOverride = StructPropertyHandle->GetProperty()->GetBoolMetaData(TEXT("sRGB"));
	}

	bDontUpdateWhileEditing = false;

	TSharedPtr<SWidget> ColorWidget;
	float ContentWidth = 125.0f;

	TWeakPtr<IPropertyHandle> StructWeakHandlePtr = StructPropertyHandle;

	ColorWidget = CreateColorWidget(StructWeakHandlePtr);

	ChildSlot
	[
		SNew(SBox)
		.MinDesiredWidth(ContentWidth)
		[
			ColorWidget.ToSharedRef()
		]
	];
}


TSharedRef<SWidget> SFactionColor::CreateColorWidget(TWeakPtr<IPropertyHandle> StructWeakHandlePtr)
{
	return SAssignNew(ColorWidgetBackgroundBorder, SBorder)
		.Padding(1)
		.BorderImage(FAppStyle::Get().GetBrush("ColorPicker.RoundedSolidBackground"))
		.BorderBackgroundColor(this, &SFactionColor::GetColorWidgetBorderColor)
		.VAlign(VAlign_Center)
		[
			SAssignNew(ColorPickerParentWidget, SColorBlock)
			.AlphaBackgroundBrush(FAppStyle::Get().GetBrush("ColorPicker.RoundedAlphaBackground"))
			.Color(this, &SFactionColor::OnGetColorForColorBlock)
			.ShowBackgroundForAlpha(true)
			.AlphaDisplayMode(bIgnoreAlpha ? EColorBlockAlphaDisplayMode::Ignore : EColorBlockAlphaDisplayMode::Separate)
			.OnMouseButtonDown(this, &SFactionColor::OnMouseButtonDownColorBlock)
			.Size(FVector2D(70.0f, 16.0f))
			.CornerRadius(FVector4(4.0f,4.0f,4.0f,4.0f))
			.IsEnabled(this, &SFactionColor::IsValueEnabled, StructWeakHandlePtr)
		];
}

void SFactionColor::CreateColorPicker(bool bUseAlpha)
{
	GEditor->BeginTransaction(FText::Format(LOCTEXT("SetColorProperty", "Edit {0}"), StructPropertyHandle->GetPropertyDisplayName()));

	SavedPreColorPickerColors.Empty();
	TArray<FString> PerObjectValues;
	StructPropertyHandle->GetPerObjectValues(PerObjectValues);

	for (int32 ObjectIndex = 0; ObjectIndex < PerObjectValues.Num(); ++ObjectIndex)
	{
		if (bIsLinearColor)
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

	const bool bRefreshOnlyOnOk = bDontUpdateWhileEditing || StructPropertyHandle->HasMetaData("DontUpdateWhileEditing");
	const bool bOnlyRefreshOnMouseUp = StructPropertyHandle->HasMetaData("OnlyUpdateOnInteractionEnd");

	FColorPickerArgs PickerArgs;
	{
		PickerArgs.bUseAlpha = !bIgnoreAlpha;
		PickerArgs.bOnlyRefreshOnMouseUp = bOnlyRefreshOnMouseUp;
		PickerArgs.bOnlyRefreshOnOk = bRefreshOnlyOnOk;
		PickerArgs.sRGBOverride = sRGBOverride;
		PickerArgs.DisplayGamma = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateUObject(GEngine, &UEngine::GetDisplayGamma));
		PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &SFactionColor::OnSetColorFromColorPicker);
		PickerArgs.OnColorPickerCancelled = FOnColorPickerCancelled::CreateSP(this, &SFactionColor::OnColorPickerCancelled);
		PickerArgs.OnColorPickerWindowClosed = FOnWindowClosed::CreateSP(this, &SFactionColor::OnColorPickerWindowClosed);
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
	if (bIsLinearColor)
	{
		LastPickerColorString = NewColor.ToString();
	}
	else
	{
		const bool bSRGB = true;
		FColor NewFColor = NewColor.ToFColor(bSRGB);
		LastPickerColorString = NewFColor.ToString();
	}

	EPropertyValueSetFlags::Type PropertyFlags = EPropertyValueSetFlags::NotTransactable;
	PropertyFlags |= bIsInteractive ? EPropertyValueSetFlags::InteractiveChange : 0;
	StructPropertyHandle->SetValueFromFormattedString(LastPickerColorString, PropertyFlags);
	StructPropertyHandle->NotifyFinishedChangingProperties();
}

void SFactionColor::ResetColors()
{
	TArray<FString> PerObjectColors;

	for (int32 ColorIndex = 0; ColorIndex < SavedPreColorPickerColors.Num(); ++ColorIndex)
	{
		if (bIsLinearColor)
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
		// See @TODO in SFactionColor::OnColorPickerWindowClosed
		// ensure(StructPropertyHandle->SetPerObjectValues(PerObjectColors, EPropertyValueSetFlags::NotTransactable) == FPropertyAccess::Success);
		StructPropertyHandle->SetPerObjectValues(PerObjectColors, EPropertyValueSetFlags::NotTransactable);
	}
}

void SFactionColor::OnColorPickerCancelled(FLinearColor OriginalColor)
{
	ResetColors();
	LastPickerColorString.Reset();

	GEditor->CancelTransaction(0);
}

void SFactionColor::OnColorPickerWindowClosed(const TSharedRef<SWindow>& Window)
{
	// Transact only at the end to avoid opening a lingering transaction. Reset value before transacting.
	if (!LastPickerColorString.IsEmpty())
	{
		//@TODO: Not using reset & apply instant scoped transition pattern since certain property nodes are 
		// returning nullptr when finding objects to modify on reset, so we can't reset correctly for those.
		// ResetColors();
		{
			// FScopedTransaction Transaction(FText::Format(LOCTEXT("SetStructPropertyHandle", "Edit {0}"), StructPropertyHandle->GetPropertyDisplayName()));
			StructPropertyHandle->SetValueFromFormattedString(LastPickerColorString);
		}
	}

	GEditor->EndTransaction();
}


void SFactionColor::OnColorPickerInteractiveBegin()
{
	bIsInteractive = true;
}


void SFactionColor::OnColorPickerInteractiveEnd()
{
	bIsInteractive = false;
}


FLinearColor SFactionColor::OnGetColorForColorBlock() const
{
	FLinearColor Color;
	GetColorAsLinear(Color);
	return Color;
}


FSlateColor SFactionColor::OnGetSlateColorForBlock() const
{
	FLinearColor Color = OnGetColorForColorBlock();
	Color.A = 1;
	return FSlateColor(Color);
}

FSlateColor SFactionColor::GetColorWidgetBorderColor() const
{
	static const FSlateColor HoveredColor = FAppStyle::Get().GetSlateColor("Colors.Hover");
	static const FSlateColor DefaultColor = FAppStyle::Get().GetSlateColor("Colors.InputOutline");
	return ColorWidgetBackgroundBorder->IsHovered() ? HoveredColor : DefaultColor;
}


FPropertyAccess::Result SFactionColor::GetColorAsLinear(FLinearColor& OutColor) const
{
	if (!StructPropertyHandle.IsValid())
		return FPropertyAccess::Fail;

	// Default to full alpha in case the alpha component is disabled.
	OutColor.A = 1.0f;

	FString StringValue;
	FPropertyAccess::Result Result = StructPropertyHandle->GetValueAsFormattedString(StringValue);

	if(Result == FPropertyAccess::Success)
	{
		if (bIsLinearColor)
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
	else if(Result == FPropertyAccess::MultipleValues)
	{
		OutColor = FLinearColor::White;
	}

	return Result;
}

FReply SFactionColor::OnMouseButtonDownColorBlock(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return FReply::Unhandled();
	}

	bool CanShowColorPicker = true;
	if (StructPropertyHandle.IsValid() && StructPropertyHandle->GetProperty() != nullptr)
	{
		CanShowColorPicker = !StructPropertyHandle->IsEditConst();
	}
	if (CanShowColorPicker)
	{
		CreateColorPicker(true /*bUseAlpha*/);
	}

	return FReply::Handled();
}

FReply SFactionColor::OnOpenFullColorPickerClicked()
{
	CreateColorPicker(true /*bUseAlpha*/);
	bIsInlineColorPickerVisible = false;

	return FReply::Handled();
}

bool SFactionColor::IsValueEnabled(TWeakPtr<IPropertyHandle> WeakHandlePtr) const
{
	return WeakHandlePtr.IsValid() && !WeakHandlePtr.Pin()->IsEditConst();
}

#undef LOCTEXT_NAMESPACE
