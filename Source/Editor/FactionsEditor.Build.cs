// Copyright 2015-2020 Piperift. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class FactionsEditor : ModuleRules
{
    public FactionsEditor(ReadOnlyTargetRules TargetRules) : base(TargetRules)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "InputCore"
			// ... add other public dependencies that you statically link with here ...
		});

        PrivateDependencyModuleNames.AddRange(new string[] {
            "AppFramework",
            "AssetTools",
            "BlueprintGraph",
            "Core",
            "CoreUObject",
            "DetailCustomizations",
            "EditorStyle",
            "Engine",
            "Factions",
            "GraphEditor",
            "Kismet",
            "PropertyEditor",
            "Settings",
            "Slate",
            "SlateCore",
            "ToolWidgets",
            "UnrealEd"
			// ... add private dependencies that you statically link with here ...
		});

        DynamicallyLoadedModuleNames.AddRange(new string[] {
			// ... add any modules that your module loads dynamically here ...
		});
    }
}
