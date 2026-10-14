// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#if __has_include("BaseCar.h")
#include "BaseCar.h"
#elif __has_include("Car/BaseCar.h")
#include "Car/BaseCar.h"
#endif
#include "TrackSupercar.generated.h"

/**
 * ATrackSupercar
 * Archetype: Pure track-focused aerodynamic hyper-weapon (e.g. McLaren Senna level).
 * 
 * Specifications:
 * - Engine: 720 HP 4.0L Twin-Turbocharged V8, 8500 RPM redline
 * - Torque: 800 N*m @ 5,500 - 6,700 RPM
 * - Curb Weight: 1200 kg (Carbon-fibre monocoque ultralightweight)
 * - Power-to-Weight: 600.0 HP / tonne
 * - Top Speed: 355 km/h
 * - Acceleration 0-100 km/h: 2.8 seconds
 * - Drivetrain: Rear-Wheel Drive (RWD) with 7-speed Seamless Shift Gearbox (SSG)
 * - Weight Bias: 43% Front / 57% Rear (Mid-engine balance)
 * - Handling: Colossal downforce, ultra-stiff hydraulic suspension, track-focused surgical grip (up to 2.2G lateral)
 * - Aerodynamics: Active dual-element rear wing with 90-degree Airbrake deployment
 */
UCLASS()
class VELOCITYX_API ATrackSupercar : public ABaseCar
{
	GENERATED_BODY()

public:
	ATrackSupercar();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Toggle Race/Track Mode (lowers ride height and stiffens dampers) */
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|TrackSupercar")
	void ToggleRaceMode();

	/** Returns whether Race Mode is actively engaged */
	UFUNCTION(BlueprintPure, Category = "VELOCITY X|TrackSupercar")
	bool IsRaceModeActive() const { return bRaceModeActive; }

protected:
	/** Track/Race Mode toggle flag */
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|TrackSupercar")
	bool bRaceModeActive = true;

	/** Active Front Aero Blade deflection angle */
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|TrackSupercar")
	float FrontAeroBladeAngle = 0.0f;

	/** High-downforce track physics simulation */
	virtual void SimulateTrackAeroAndGrip(float DeltaTime);
};
