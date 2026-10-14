// Copyright VELOCITY X Racing. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class VELOCITYXTarget : TargetRules
{
	public VELOCITYXTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange(new string[] { "VELOCITYX" });
	}
}
