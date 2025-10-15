// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MrPinstripe : ModuleRules
{
	public MrPinstripe(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" , "UMG", "Slate", "SlateCore", "Niagara",
		"AnimGraphRuntime", "AIModule", "GameplayTasks", "NavigationSystem", "RenderCore", "DeveloperSettings", "MovieScene", "MovieSceneTracks",
		"ApplicationCore", "Json", "JsonUtilities", "RHI"});
    }
}
