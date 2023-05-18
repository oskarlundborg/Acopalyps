// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Acopalyps : ModuleRules
{
	public Acopalyps(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "GameplayTasks", "UMG", "Niagara", "NavigationSystem" });

		CppStandard = CppStandardVersion.Cpp20;
		OptimizeCode = CodeOptimization.Never; // Maybe change this, improved debugging for me but might mess other things up.
	}
}
