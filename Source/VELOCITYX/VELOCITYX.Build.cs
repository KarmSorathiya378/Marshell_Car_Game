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
			"ChaosVehicles",
			"PhysicsCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] 
		{ 
			"Slate", 
			"SlateCore",
			"Projects",
			"Json",
			"JsonUtilities"
		});

		PublicIncludePaths.AddRange(new string[]
		{
			Path.Combine(ModuleDirectory, "Public"),
			Path.Combine(ModuleDirectory, "Public", "Vehicle"),
			Path.Combine(ModuleDirectory, "Public", "Telemetry"),
			Path.Combine(ModuleDirectory, "Public", "GameInstance"),
			Path.Combine(ModuleDirectory, "Vehicle"),
			Path.Combine(ModuleDirectory, ".."),
			Path.Combine(ModuleDirectory, "..", "Car"),
			Path.Combine(ModuleDirectory, "..", "Input", "Public")
		});

		PrivateIncludePaths.AddRange(new string[]
		{
			Path.Combine(ModuleDirectory, "Private"),
			Path.Combine(ModuleDirectory, "Private", "Vehicle"),
			Path.Combine(ModuleDirectory, "Private", "Telemetry"),
			Path.Combine(ModuleDirectory, "Private", "GameInstance"),
			Path.Combine(ModuleDirectory, "..", "Input", "Private")
		});
	}
}
