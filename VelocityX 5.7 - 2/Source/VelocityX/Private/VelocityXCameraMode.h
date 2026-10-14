#pragma once

#include "CoreMinimal.h"
#include "VelocityXCameraMode.generated.h"

UENUM(BlueprintType)
enum class EVelocityXCameraMode : uint8
{
	Chase UMETA(DisplayName = "Chase View (Third-Person)"),
	Hood UMETA(DisplayName = "Hood / Bumper View"),
	Cockpit UMETA(DisplayName = "Cockpit / First-Person View")
};
