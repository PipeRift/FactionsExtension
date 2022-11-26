// Copyright 2015-2020 Piperift. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <Styling/SlateStyle.h>


/** Manages the style which provides resources for UtilityTree editor widgets. */
class FACTIONSEDITOR_API FFactionsEditorStyle
{
public:

	static void Initialize();

	static void Shutdown();

	/** @return The Slate style set for UtilityTree editor widgets */
	static const TSharedPtr<class ISlateStyle> Get();

	static FName GetStyleSetName();

private:

	static FString InContent(const FString& RelativePath, const ANSICHAR* Extension);

	static void Create();


	static TSharedPtr<class FSlateStyleSet> StyleSet;

public:

	static const FSlateBrush* GetBrush(FName PropertyName, const ANSICHAR* Specifier = NULL)
	{
		return StyleSet->GetBrush(PropertyName, Specifier);
	}
};