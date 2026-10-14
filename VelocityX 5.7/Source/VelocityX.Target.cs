// Copyright VELOCITY X Racing. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class VelocityXTarget : TargetRules
{
	public VelocityXTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange(new string[] { "VelocityX" });
	}
}
