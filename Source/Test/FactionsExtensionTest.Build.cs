// Copyright 2015-2026 Piperift. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class FactionsExtensionTest : ModuleRules
    {
        public FactionsExtensionTest(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicDependencyModuleNames.AddRange(new string[]
            {
                "Core"
            });

            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "AIModule",
                "CoreUObject",
                "Engine",
                "EngineSettings",
                "FactionsExtension"
            });

            if (Target.bBuildEditor == true)
            {
                PrivateDependencyModuleNames.Add("UnrealEd");
            }
        }
    }
}