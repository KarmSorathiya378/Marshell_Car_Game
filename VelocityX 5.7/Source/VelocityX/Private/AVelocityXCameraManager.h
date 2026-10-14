// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AVelocityXCameraManager.generated.h"

class AVelocityXCar;
class USpringArmComponent;
class UCameraComponent;

/** 3 Distinct Camera Modes for VELOCITY X */
UENUM(BlueprintType)
enum class EVelocityXCameraMode : uint8
{
	Chase UMETA(DisplayName = "Chase View (Third-Person)"),
	Hood UMETA(DisplayName = "Hood / Bumper View"),
	Cockpit UMETA(DisplayName = "Cockpit / First-Person View")
};

/**
 * AVelocityXCameraManager
 * Comprehensive racing camera system featuring:
 * - 3 camera modes (Chase, Hood, Cockpit)
 * - Smooth camera blending and transitions
 * - Dynamic speed-dependent Field of View (FOV)
 * - Optional high-speed camera shake and turbulence vibration
 */
UCLASS()
class VELOCITYX_API AVelocityXCameraManager : public AActor
{
	GENERATED_BODY()

public:
	AVelocityXCameraManager();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// --- Target Vehicle ---
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Camera")
	void SetTargetVehicle(AVelocityXCar* NewVehicle);

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Camera")
	AVelocityXCar* GetTargetVehicle() const { return TargetVehicle; }

	// --- Camera Mode Switching ---
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Camera")
	void SetCameraMode(EVelocityXCameraMode NewMode, bool bSnapImmediate = false);

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Camera")
	void CycleCameraMode();

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Camera")
	EVelocityXCameraMode GetCurrentCameraMode() const { return CurrentMode; }

	// --- Camera Configuration Parameters ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Camera|FOV")
	float BaseFOV = 85.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Camera|FOV")
	float MaxFOV = 108.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Camera|FOV")
	float SpeedForMaxFOVKph = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Camera|Transitions")
	float TransitionSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Camera|Shake")
	bool bEnableHighSpeedShake = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Camera|Shake")
	float HighSpeedShakeThresholdKph = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Camera|Shake")
	float MaxShakeIntensity = 1.6f;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VELOCITY X|Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VELOCITY X|Camera")
	UCameraComponent* MainCamera;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Camera")
	AVelocityXCar* TargetVehicle = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Camera")
	EVelocityXCameraMode CurrentMode = EVelocityXCameraMode::Chase;

	// Internal transition states
	FVector DesiredCameraLocation = FVector::ZeroVector;
	FRotator DesiredCameraRotation = FRotator::ZeroRotator;
	float CurrentTargetArmLength = 450.0f;
	float CurrentFOV = 85.0f;
	float ShakeTimer = 0.0f;

	virtual void UpdateCameraModeTransform(float DeltaTime);
	virtual void UpdateDynamicFOV(float DeltaTime);
	virtual void ApplyHighSpeedShake(float DeltaTime, FVector& OutLocation, FRotator& OutRotation);
};
