// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Vehicle/BaseCar.h"
#include "Supercar.generated.h"

/**
 * Supercar - Balanced high-performance street vehicle
 * Optimized for street handling, quick acceleration, and daily supercar driving dynamics.
 */
UCLASS()
class VELOCITYX_API ASupercar : public ABaseCar
{
	GENERATED_BODY()

public:
	ASupercar();

	// --- Supercar Specific Features ---
	UFUNCTION(BlueprintCallable, Category = "Supercar")
	void ToggleSportMode();

	UFUNCTION(BlueprintPure, Category = "Supercar")
	bool IsSportModeActive() const { return bSportMode; }

protected:
	virtual void SetupCarDefaults() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supercar")
	bool bSportMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supercar")
	float SportModeThrottleMultiplier;
};
