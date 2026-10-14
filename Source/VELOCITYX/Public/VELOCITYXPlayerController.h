// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VelocityPlayerController.h"
#include "VELOCITYXPlayerController.generated.h"

/**
 * VELOCITY X Player Controller Alias / Extension
 * Inherits all driving controls, camera management, and input logic from AVelocityPlayerController.
 */
UCLASS()
class VELOCITYX_API AVELOCITYXPlayerController : public AVelocityPlayerController
{
	GENERATED_BODY()

public:
	AVELOCITYXPlayerController();
};
