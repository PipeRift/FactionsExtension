// Copyright 2015-2020 Piperift. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class FactionsTest : ModuleRules
    {
        public FactionsTest(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
            bEnforceIWYU = true;

            PublicDependencyModuleNames.AddRange(new string[]
            {
                "Core",
                "Engine",
                "CoreUObject",
                "Factions",
                "AIModule"
            });

            if (Target.bBuildEditor == true)
            {
                PrivateDependencyModuleNames.Add("UnrealEd");
            }
        }
    }
}