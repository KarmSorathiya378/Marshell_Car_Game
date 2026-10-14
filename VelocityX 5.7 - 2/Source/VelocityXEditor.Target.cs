// Copyright VELOCITY X Racing. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class VelocityXEditorTarget : TargetRules
{
	public VelocityXEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange(new string[] { "VelocityX" });
	}
}
