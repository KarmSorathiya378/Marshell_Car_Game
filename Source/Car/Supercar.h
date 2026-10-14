// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#if __has_include("BaseCar.h")
#include "BaseCar.h"
#elif __has_include("Car/BaseCar.h")
#include "Car/BaseCar.h"
#endif
#include "Supercar.generated.h"

/**
 * ASupercar
 * Archetype: High-performance naturally aspirated sports icon (e.g. Porsche 911 GT3 RS level).
 * 
 * Specifications:
 * - Engine: 500 HP naturally aspirated 4.0L Boxer-6, 9000 RPM redline
 * - Torque: 470 N*m @ 6,300 RPM
 * - Curb Weight: 1500 kg
 * - Power-to-Weight: 333.3 HP / tonne
 * - Top Speed: 320 km/h
 * - Acceleration 0-100 km/h: 3.5 seconds
 * - Drivetrain: Rear-Wheel Drive (RWD) with 7-speed dual-clutch PDK transmission
 * - Weight Bias: 39% Front / 61% Rear (rear-engine bias)
 * - Handling: Balanced mechanical grip with playful, communicative lift-off oversteer
 * - Aerodynamics: Swan-neck active rear wing with Drag Reduction System (DRS)
 */
UCLASS()
class VELOCITYX_API ASupercar : public ABaseCar
{
	GENERATED_BODY()

public:
	ASupercar();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Toggle Drag Reduction System (DRS) wing flap */
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Supercar")
	void ToggleDRS();

	/** Check if DRS flap is open */
	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Supercar")
	bool IsDRSOpen() const { return bDRSOpen; }

protected:
	/** State of Drag Reduction System (DRS) flap */
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Supercar")
	bool bDRSOpen = false;

	/** Oversteer drift angle helper */
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Supercar")
	float DriftAngleDegrees = 0.0f;

	/** Custom oversteer handling physics tweak */
	virtual void SimulateSupercarHandling(float DeltaTime);
};
