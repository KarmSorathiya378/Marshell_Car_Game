// Copyright VELOCITY X Racing. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class VELOCITYXEditorTarget : TargetRules
{
	public VELOCITYXEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange(new string[] { "VELOCITYX" });
	}
}
