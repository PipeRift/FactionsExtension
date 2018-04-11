// Copyright 2015-2018 Piperift. All Rights Reserved.

using UnrealBuildTool;

public class FactionsEditor : ModuleRules
{
	public FactionsEditor(ReadOnlyTargetRules TargetRules) : base(TargetRules)
    {
		
		PublicIncludePaths.AddRange(
			new string[] {
                "Editor/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"Editor/Private"
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "InputCore"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
				"CoreUObject",
				"Engine",
                "UnrealEd",
                "Blutility",
				"Slate",
				"SlateCore",
                "AssetTools",
                "EditorStyle",
                "KismetWidgets",
                "KismetCompiler",
                "BlueprintGraph",
                "GraphEditor",
                "Kismet",
                "PropertyEditor",
                "DetailCustomizations",
                "ContentBrowser",
                "Settings",
                "Factions"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
