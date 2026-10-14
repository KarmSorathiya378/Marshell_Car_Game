// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#if __has_include("BaseCar.h")
#include "BaseCar.h"
#elif __has_include("Car/BaseCar.h")
#include "Car/BaseCar.h"
#endif
#include "Hypercar.generated.h"

/** Aerodynamic mode for active hypercar rear wing */
UENUM(BlueprintType)
enum class EHypercarAeroMode : uint8
{
	Autobahn UMETA(DisplayName = "Autobahn / Standard Mode"),
	Handling UMETA(DisplayName = "Handling Mode (High Downforce)"),
	TopSpeed UMETA(DisplayName = "Top Speed Key Mode (Low Drag 420 km/h)"),
	Airbrake UMETA(DisplayName = "Airbrake Mode")
};

/**
 * AHypercar
 * Archetype: The pinnacle of power, luxury, and ballistic top speed (e.g. Bugatti Chiron level).
 * 
 * Specifications:
 * - Engine: 1500 HP 8.0L Quad-Turbocharged W16, 7000 RPM redline
 * - Torque: 1600 N*m @ 2,000 - 6,000 RPM flat plateau
 * - Curb Weight: 1900 kg
 * - Power-to-Weight: 789.5 HP / tonne
 * - Top Speed: 420 km/h (electronically governed)
 * - Acceleration 0-100 km/h: 2.4 seconds
 * - Drivetrain: Permanent All-Wheel Drive (AWD) with 30% Front / 70% Rear torque split
 * - Transmission: Heavy-duty 7-speed Dual-Clutch with active torque vectoring
 * - Weight Bias: 44% Front / 56% Rear
 * - Handling: Extreme relentless thrust, launch control AWD catapult, active stability management
 * - Aerodynamics: Multi-mode active hydraulic rear wing (Autobahn, Handling, Top Speed Key, Airbrake)
 */
UCLASS()
class VELOCITYX_API AHypercar : public ABaseCar
{
	GENERATED_BODY()

public:
	AHypercar();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Engage Top Speed Key mode to unlock 420 km/h */
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Hypercar")
	void EngageTopSpeedMode();

	/** Disengage Top Speed Key mode back to Handling / Autobahn mode */
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Hypercar")
	void DisengageTopSpeedMode();

	/** Get current active aerodynamic mode */
	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Hypercar")
	EHypercarAeroMode GetAeroMode() const { return CurrentAeroMode; }

	/** Trigger Launch Control catapult acceleration */
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Hypercar")
	void ArmLaunchControl();

protected:
	/** Active aero wing mode */
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Hypercar")
	EHypercarAeroMode CurrentAeroMode = EHypercarAeroMode::Autobahn;

	/** Whether Top Speed Key is inserted */
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Hypercar")
	bool bTopSpeedKeyActive = false;

	/** Launch Control armed state */
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Hypercar")
	bool bLaunchControlArmed = false;

	/** Quad-turbo boost pressure in bar (0.0 to 1.85 bar) */
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Hypercar")
	float TurboBoostBar = 0.0f;

	/** Hypercar stability & AWD simulation */
	virtual void SimulateAWDAndStability(float DeltaTime);

	/** Dynamic aero wing positioning based on speed and brake input */
	virtual void UpdateHypercarAeroWing(float DeltaTime);
};
