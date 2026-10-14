// Copyright VELOCITY X Racing. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class VELOCITYX : ModuleRules
{
	public VELOCITYX(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"UMG",
			"GameplayTags",
			"ChaosVehicle",
			"ChaosPhysics",
			"PhysicsCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] 
		{ 
			"Slate", 
			"SlateCore",
			"Projects",
			"Json"
		});

		// Enable optimizations
		if (Target.Type == TargetType.Game)
		{
			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
			PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
		}
	}
}
