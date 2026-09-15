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
				, "DataAssetsLoader" // Created USbDataAsset
			}
		);

		PrivateDependencyModuleNames.AddRange(new[]
			{
				"CoreUObject", "Engine", "Slate", "SlateCore" // Core
				, "UMG" // UUserWidget creation
				, "GameplayAbilities" // USbBlinkAbility
				, "GameplayTags" // FGameplayTag
				, "Mover" // Used for blink teleportation
				, "Niagara" // Used for blink portals
				// Bomber modules
				, "Bomber"
				, "GameFeaturePluginsManager" // Used for GfpmUtils
				, "MyUtils",
			}
		);
	}
}
