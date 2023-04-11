// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Acopalyps : ModuleRules
{
	public Acopalyps(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "GameplayTasks", "UMG" });
	}
}
