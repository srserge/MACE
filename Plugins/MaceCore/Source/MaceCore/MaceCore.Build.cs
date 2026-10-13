// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MaceCore : ModuleRules
{
	public MaceCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"EnhancedInput",
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
				"ModularGameplay",
				"NetCore"
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"PhysicsCore"
			}
			);

		// Forbidden in this module, by design:
		//   PoseSearch, Chooser, BlendStack, AnimationWarpingRuntime,
		//   MotionWarping, Mover, ChaosMover, Locomotor, DrawDebugLibrary, UMG
		// They belong in MaceAnimation. Needing one here means the
		// abstraction is in the wrong place.
	}
}
