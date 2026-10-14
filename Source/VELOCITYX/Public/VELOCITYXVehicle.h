// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Vehicle/BaseCar.h"
#include "VELOCITYXVehicle.generated.h"

/**
 * VELOCITY X Vehicle Class
 * Backward-compatible vehicle class inheriting from ABaseCar.
 */
UCLASS()
class VELOCITYX_API AVELOCITYXVehicle : public ABaseCar
{
	GENERATED_BODY()

public:
	AVELOCITYXVehicle();
};
