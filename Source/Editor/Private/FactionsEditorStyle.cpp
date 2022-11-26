// Copyright 2015-2020 Piperift. All Rights Reserved.

#include "FactionsEditorStyle.h"

#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"
#include "Styling/StyleColors.h"
#include "EditorStyleSet.h"
#include "Interfaces/IPluginManager.h"
#include "SlateOptMacros.h"


TSharedPtr<FSlateStyleSet> FFactionsEditorStyle::StyleSet = nullptr;


void FFactionsEditorStyle::Initialize()
{
	if (!StyleSet.IsValid())
	{
		Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
	}
}

void FFactionsEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);
	ensure(StyleSet.IsUnique());
	StyleSet.Reset();
}

FName FFactionsEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("FactionsEditorStyle"));
	return StyleSetName;
}

FString FFactionsEditorStyle::InContent(const FString& RelativePath, const ANSICHAR* Extension)
{
	static FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("FactionsExtension"))->GetContentDir();
	const FString FinalPath = (ContentDir / TEXT("Editor") / RelativePath) + Extension;
	return FinalPath;
}

#define IMAGE_PLUGIN_BRUSH( RelativePath, ... ) FSlateImageBrush( FFactionsEditorStyle::InContent( TEXT(RelativePath), ".png" ), __VA_ARGS__ )
#define BOX_PLUGIN_BRUSH( RelativePath, ... ) FSlateBoxBrush( FFactionsEditorStyle::InContent( TEXT(RelativePath), ".png" ), __VA_ARGS__ )
#define IMAGE_BRUSH( RelativePath, ...) FSlateImageBrush( StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BOX_BRUSH( RelativePath, ...) FSlateBoxBrush( StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define TTF_PLUGIN_FONT( RelativePath, ... ) FSlateFontInfo( FFactionsEditorStyle::InContent( TEXT(RelativePath), ".png" ), __VA_ARGS__ )
#define TTF_CORE_FONT( RelativePath, ... ) FSlateFontInfo( StyleSet->RootToCoreContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FFactionsEditorStyle::Create()
{
	// Const icon sizes
	static const FVector2D Icon8x8(8.0f, 8.0f);
	static const FVector2D Icon10x10(10.0f, 10.0f);
	static const FVector2D Icon16x16(16.0f, 16.0f);
	static const FVector2D Icon20x20(20.0f, 20.0f);
	static const FVector2D Icon32x32(32.0f, 32.0f);
	static const FVector2D Icon40x40(40.0f, 40.0f);
	static const FVector2D Icon64x64(64.0f, 64.0f);

	StyleSet = MakeShared<FSlateStyleSet>(GetStyleSetName());

	StyleSet->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));
	StyleSet->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	const FTextBlockStyle& NormalText = FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");

	StyleSet->Set("Relation.Directional",   new IMAGE_PLUGIN_BRUSH("arrow_right_16x", Icon16x16));
	StyleSet->Set("Relation.Bidirectional", new IMAGE_PLUGIN_BRUSH("arrow_right_left_16x", Icon16x16));

	const float CheckboxCornerRadius = 3.f;
	StyleSet->Set("Relation.DirectionalCheckBox", FCheckBoxStyle()
		.SetCheckBoxType(ESlateCheckBoxType::CheckBox)
		.SetUncheckedImage(IMAGE_PLUGIN_BRUSH("arrow_right_32x", Icon20x20))
		.SetUncheckedHoveredImage(IMAGE_PLUGIN_BRUSH("arrow_right_32x", Icon20x20))
		.SetUncheckedPressedImage(IMAGE_PLUGIN_BRUSH("arrow_right_32x", Icon20x20))
		.SetCheckedImage(IMAGE_PLUGIN_BRUSH("arrow_right_left_32x", Icon20x20))
		.SetCheckedHoveredImage(IMAGE_PLUGIN_BRUSH("arrow_right_left_32x", Icon20x20))
		.SetCheckedPressedImage(IMAGE_PLUGIN_BRUSH("arrow_right_left_32x", Icon20x20))
		.SetForegroundColor(FStyleColors::Foreground)
		.SetHoveredForegroundColor(FStyleColors::ForegroundHover)
		.SetPressedForegroundColor(FStyleColors::ForegroundHover)
		.SetCheckedForegroundColor(FStyleColors::Foreground)
		.SetCheckedHoveredForegroundColor(FStyleColors::ForegroundHover)
		.SetCheckedPressedForegroundColor(FStyleColors::ForegroundHover));

	StyleSet->Set("Relation.TransparentRoundedBox", new FSlateRoundedBoxBrush(FStyleColors::Transparent, CheckboxCornerRadius, Icon20x20));

}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef IMAGE_PLUGIN_BRUSH
#undef BOX_PLUGIN_BRUSH
#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef TTF_PLUGIN_FONT
#undef TTF_CORE_FONT

const TSharedPtr<ISlateStyle> FFactionsEditorStyle::Get()
{
	return StyleSet;
}
#undef TTF_FONT