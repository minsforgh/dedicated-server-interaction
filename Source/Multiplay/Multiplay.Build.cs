// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Multiplay : ModuleRules
{
	public Multiplay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Slate", "SlateCore", "NetCore" });

        PublicIncludePaths.AddRange(
            new string[] {
                "Multiplay",
                "Multiplay/UI",
                "Multiplay/Chatting",
                "Multiplay/Trading",
                "Multiplay/Inventory",
                "Multiplay/Characters",
                "Multiplay/Controllers",
                "Multiplay/Modes"
    }
);
    }
}

