// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#if __has_include("WheeledVehiclePawn.h")
#include "WheeledVehiclePawn.h"
#ifndef AWheeledVehicle
#define AWheeledVehicle AWheeledVehiclePawn
#endif
#elif __has_include("WheeledVehicle.h")
#include "WheeledVehicle.h"
#elif __has_include("ChaosVehicle.h")
#include "ChaosVehicle.h"
#ifndef AWheeledVehicle
#define AWheeledVehicle AChaosVehicle
#endif
#else
#include "GameFramework/Pawn.h"
#ifndef AWheeledVehicle
#define AWheeledVehicle APawn
#endif
#endif

#include "Curves/CurveFloat.h"
#include "AVelocityXChaosVehicle.generated.h"

/** Wheel configuration struct defining geometry, spring rates, and friction */
USTRUCT(BlueprintType)
struct FVelocityXWheelConfig
{
	GENERATED_BODY()

	/** Outer rolling radius of the wheel in centimeters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel")
	float WheelRadius = 35.0f;

	/** Tire section width in millimeters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel")
	float TireWidth = 285.0f;

	/** Peak friction coefficient (mu) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel")
	float FrictionMultiplier = 1.70f;

	/** Cornering stiffness parameter for lateral grip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel")
	float CorneringStiffness = 20.0f;

	/** Longitudinal stiffness parameter for acceleration and braking grip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheel")
	float LongitudinalStiffness = 24.0f;

	/** Suspension spring stiffness in N/m */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float SuspensionStiffness = 90000.0f;

	/** Shock absorber compression damping in N*s/m */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float DampingCompression = 5000.0f;

	/** Shock absorber rebound damping in N*s/m */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float DampingRebound = 7500.0f;

	/** Maximum suspension raise / bump stroke travel (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float SuspensionMaxRaise = 8.0f;

	/** Maximum suspension drop / droop stroke travel (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float SuspensionMaxDrop = 8.0f;

	/** Whether this wheel receives steering input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steering")
	bool bSteerable = false;

	/** Whether this wheel is driven by the powertrain */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drivetrain")
	bool bDriven = false;
};

/** Individual wheel dynamic runtime state */
USTRUCT(BlueprintType)
struct FVelocityXWheelRuntimeState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	int32 WheelIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float AngularVelocity = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float RotationDegrees = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float SteerAngleDegrees = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float SuspensionCompressionCm = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float VerticalLoadN = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float SlipRatio = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float SlipAngleDegrees = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float TractionForceX = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float CorneringForceY = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	bool bIsSkidding = false;
};

/**
 * AVelocityXChaosVehicle
 * Extended Chaos vehicle pawn featuring custom 4-wheel simulation,
 * torque curves, transmission gearings, suspension geometry, and engine RPM dynamics.
 */
UCLASS()
class VELOCITYX_API AVelocityXChaosVehicle : public AWheeledVehicle
{
	GENERATED_BODY()

public:
	AVelocityXChaosVehicle();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// --- Driver Inputs ---
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Input")
	virtual void SetThrottle(float Value);

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Input")
	virtual void SetBrake(float Value);

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Input")
	virtual void SetSteering(float Value);

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Input")
	virtual void SetHandbrake(bool bActive);

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Transmission")
	virtual void ShiftUp();

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Transmission")
	virtual void ShiftDown();

	// --- Telemetry & State Getters ---
	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	float GetSpeedKph() const { return CurrentSpeedKph; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	float GetSpeedMph() const { return CurrentSpeedKph * 0.621371f; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	float GetEngineRPM() const { return CurrentRPM; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	float GetMaxRPM() const { return RedlineRPM; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	int32 GetCurrentGear() const { return CurrentGear; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	FVelocityXWheelRuntimeState GetWheelRuntimeState(int32 WheelIndex) const;

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	float GetLongitudinalG() const { return LongitudinalG; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	float GetLateralG() const { return LateralG; }

protected:
	// --- Engine & Powertrain Parameters ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Engine")
	float Horsepower = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Engine")
	float PeakTorqueNm = 750.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Engine")
	float IdleRPM = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Engine")
	float RedlineRPM = 8500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Engine")
	float ThrottleResponseRate = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Engine")
	float EngineBrakingFactor = 0.22f;

	// --- Transmission & Gearing ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Transmission")
	TArray<float> ForwardGearRatios = { 3.82f, 2.36f, 1.69f, 1.31f, 1.10f, 0.92f, 0.78f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Transmission")
	float ReverseGearRatio = 3.50f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Transmission")
	float FinalDriveRatio = 3.73f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Transmission")
	float GearShiftTime = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Transmission")
	bool bAutomaticTransmission = true;

	// --- Wheel Setup (0: FL, 1: FR, 2: RL, 3: RR) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Wheels")
	FVelocityXWheelConfig WheelsConfig[4];

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Wheels")
	FVelocityXWheelRuntimeState WheelsState[4];

	// --- Chassis & Steering Geometry ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Chassis")
	float VehicleMassKg = 1450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Chassis")
	float WheelbaseCm = 265.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Chassis")
	float TrackWidthCm = 170.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Chassis")
	float MaxSteerAngle = 35.0f;

	// --- Dynamic Runtime Variables ---
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Runtime")
	float RawThrottle = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Runtime")
	float RawBrake = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Runtime")
	float RawSteering = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Runtime")
	bool bHandbrakeActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Runtime")
	float CurrentRPM = 900.0f;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Runtime")
	int32 CurrentGear = 1;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Runtime")
	float CurrentSpeedKph = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Runtime")
	float LongitudinalG = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Runtime")
	float LateralG = 0.0f;

	bool bIsShifting = false;
	float ShiftCooldownTimer = 0.0f;
	FVector PreviousFrameVelocity = FVector::ZeroVector;

	// Physics sub-routines
	virtual void SimulateEngine(float DeltaTime);
	virtual void SimulateWheelsAndSuspension(float DeltaTime);
	virtual void SimulateTransmission(float DeltaTime);
	virtual void CalculateVehicleDynamics(float DeltaTime);
	virtual float GetTorqueAtRPM(float RPM) const;
};
