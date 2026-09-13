// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SkillBlinkRuntime : ModuleRules
{
	public SkillBlinkRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppCompileWarningSettings.NonInlinedGenCppWarningLevel = WarningLevel.Error;
		
		PublicDependencyModuleNames.AddRange(new[]
			{
				"Core"
				// Bomber modules
				, "MetaCheatManager" // USbCheatExtension
			}
		);

		PrivateDependencyModuleNames.AddRange(new[]
			{
				"CoreUObject", "Engine", "Slate", "SlateCore" // Core
				, "GameplayAbilities" // USbBlinkAbility
				// Bomber modules
				, "Bomber"
			}
		);
	}
}
