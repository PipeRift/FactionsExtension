// Copyright 2015-2018 Piperift. All Rights Reserved.

using UnrealBuildTool;

public class Factions : ModuleRules
{
	public Factions(ReadOnlyTargetRules TargetRules) : base(TargetRules)
    {
		PublicIncludePaths.AddRange(new string[]
        {
            "Factions/Public"
			// ... add public include paths required here ...
	    });
				
		
		PrivateIncludePaths.AddRange(new string[]
        {
			"Factions/Private"
			// ... add other private include paths required here ...
		});
			
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
            "GameplayTasks",
            "AIModule",
            "MovieSceneTracks"
			// ... add other public dependencies that you statically link with here ...
		});

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "GameplayTags"
        });


        if (TargetRules.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "SlateCore",
                    "Slate"
                }
            );
        }

        if (TargetRules.bBuildDeveloperTools || (Target.Configuration != UnrealTargetConfiguration.Shipping && Target.Configuration != UnrealTargetConfiguration.Test))
        {
            PrivateDependencyModuleNames.Add("GameplayDebugger");
            Definitions.Add("WITH_GAMEPLAY_DEBUGGER=1");
        }
        else
        {
            Definitions.Add("WITH_GAMEPLAY_DEBUGGER=0");
        }
    }
}
