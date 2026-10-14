// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "BaseCar.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UChaosWheeledVehicleMovementComponent;

/**
 * Performance specifications and tuning parameters for vehicles
 */
USTRUCT(BlueprintType)
struct FCarPerformanceStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Specification")
	FString ModelName = TEXT("Base Car");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Specification")
	float Horsepower = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Specification")
	float WeightKg = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
	float MaxSpeedMPH = 190.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
	float Acceleration0to60 = 3.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
	float BrakeForce = 3500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handling")
	float SteerSensitivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handling")
	float GripFactor = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics")
	float DragCoefficient = 0.32f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics")
	float DownforceMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	int32 NumGears = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float EngineTorque = 550.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float RedlineRPM = 7500.0f;
};

/**
 * Base Car class for VELOCITY X
 * Foundation for all vehicle types utilizing UE5 Chaos Vehicle physics.
 */
UCLASS()
class VELOCITYX_API ABaseCar : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	ABaseCar();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// --- Vehicle Controls ---
	UFUNCTION(BlueprintCallable, Category = "Vehicle Input")
	virtual void ApplyThrottle(float Value);

	UFUNCTION(BlueprintCallable, Category = "Vehicle Input")
	virtual void ApplySteering(float Value);

	UFUNCTION(BlueprintCallable, Category = "Vehicle Input")
	virtual void ApplyBrake(float Value);

	UFUNCTION(BlueprintCallable, Category = "Vehicle Input")
	virtual void ApplyHandbrake(bool bHandbrake);

	// --- Vehicle Utilities ---
	UFUNCTION(BlueprintCallable, Category = "Vehicle Action")
	virtual void ResetVehicle();

	UFUNCTION(BlueprintCallable, Category = "Vehicle Action")
	virtual void ToggleHeadlights();

	UFUNCTION(BlueprintCallable, Category = "Vehicle Camera")
	virtual void CycleCameraMode();

	// --- Telemetry Getters ---
	UFUNCTION(BlueprintPure, Category = "Vehicle Telemetry")
	float GetSpeedMPH() const;

	UFUNCTION(BlueprintPure, Category = "Vehicle Telemetry")
	float GetSpeedKPH() const;

	UFUNCTION(BlueprintPure, Category = "Vehicle Telemetry")
	float GetEngineRPM() const;

	UFUNCTION(BlueprintPure, Category = "Vehicle Telemetry")
	int32 GetCurrentGear() const;

	UFUNCTION(BlueprintPure, Category = "Vehicle Telemetry")
	const FCarPerformanceStats& GetPerformanceStats() const { return PerformanceStats; }

	UFUNCTION(BlueprintPure, Category = "Vehicle Telemetry")
	bool AreHeadlightsActive() const { return bHeadlightsOn; }

	// --- Performance Profile ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Stats")
	FCarPerformanceStats PerformanceStats;

	// --- Camera Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* ChaseCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* ChaseCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CockpitCamera;

protected:
	virtual void SetupCarDefaults();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UChaosWheeledVehicleMovementComponent* ChaosVehicleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bHeadlightsOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	int32 ActiveCameraIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Telemetry")
	float CurrentForwardSpeed;
};
