// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE5_EOSTesting : ModuleRules
{
	public UE5_EOSTesting(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemEOS"
		});
	}
}
