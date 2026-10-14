// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Vehicle/BaseCar.h"
#include "Hypercar.generated.h"

/**
 * Hypercar - Pinnacle performance engineering
 * Quad-turbo hybrid powertrain, active aerodynamics, extreme 280+ MPH top speed and electric torque fill.
 */
UCLASS()
class VELOCITYX_API AHypercar : public ABaseCar
{
	GENERATED_BODY()

public:
	AHypercar();

	virtual void Tick(float DeltaTime) override;

	// --- Hypercar Hybrid / Boost System ---
	UFUNCTION(BlueprintCallable, Category = "Hypercar Boost")
	void SetHybridBoostActive(bool bActive);

	UFUNCTION(BlueprintPure, Category = "Hypercar Boost")
	bool IsHybridBoostActive() const { return bBoostActive; }

	UFUNCTION(BlueprintPure, Category = "Hypercar Boost")
	float GetBoostChargePercent() const { return BoostCharge / MaxBoostCharge; }

protected:
	virtual void SetupCarDefaults() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hypercar Boost")
	bool bBoostActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hypercar Boost")
	float BoostCharge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hypercar Boost")
	float MaxBoostCharge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hypercar Boost")
	float BoostDepletionRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hypercar Boost")
	float BoostRechargeRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hypercar Boost")
	float BaseTorque;
};
