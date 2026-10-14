// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "VelocityInputConfig.h"
#include "VelocityCameraManager.generated.h"

class AVELOCITYXVehicle;
class ABaseCar;
class UVelocityHighSpeedCameraShake;
class UVelocityImpactCameraShake;

/**
 * Configuration settings for an individual camera mode in VELOCITY X
 */
USTRUCT(BlueprintType)
struct FVelocityCameraPreset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Preset")
	FVector RelativeOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Preset")
	FVector LookAtOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Preset")
	float BaseFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Preset")
	float PositionLagSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Preset")
	float RotationLagSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Preset")
	float SpeedPullbackDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Preset")
	bool bEnableCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Preset")
	bool bFollowYawOnly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Preset")
	FName PreferredMeshSocket;

	FVelocityCameraPreset()
		: RelativeOffset(FVector(-500.0f, 0.0f, 160.0f))
		, LookAtOffset(FVector(60.0f, 0.0f, 50.0f))
		, BaseFOV(85.0f)
		, PositionLagSpeed(10.0f)
		, RotationLagSpeed(8.0f)
		, SpeedPullbackDistance(120.0f)
		, bEnableCollision(true)
		, bFollowYawOnly(false)
		, PreferredMeshSocket(NAME_None)
	{
	}
};

/**
 * BP_CameraManager / AVelocityCameraManager / AVELOCITYXCameraManager
 * Implements 4-Mode Camera Manager:
 * - ChaseClose: Smooth follow behind car
 * - ChaseFar: Elevated further follow / top overview
 * - Hood: Front hood driver view (supports Socket_HoodCamera)
 * - Cockpit: Interior cabin driver view (supports Socket_CockpitCamera)
 * Supports dynamic speed FOV (90 at 0 mph up to 115-120 at 250+ mph),
 * smooth Hermite blending, collision sweep, and collision/wind/redline shake triggers.
 */
UCLASS()
class VELOCITYX_API AVelocityCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	AVelocityCameraManager();

	virtual void BeginPlay() override;
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

	// --- Mode Controls ---
	UFUNCTION(BlueprintCallable, Category = "Velocity Camera")
	void CycleCameraMode();

	UFUNCTION(BlueprintCallable, Category = "Velocity Camera")
	void SetCameraMode(EVelocityCameraMode NewMode, bool bInstant = false);

	UFUNCTION(BlueprintPure, Category = "Velocity Camera")
	EVelocityCameraMode GetCameraMode() const { return CurrentCameraMode; }

	// Target vehicle assignment
	UFUNCTION(BlueprintCallable, Category = "Velocity Camera")
	void SetTargetVehicle(AVELOCITYXVehicle* InVehicle);

	UFUNCTION(BlueprintCallable, Category = "Velocity Camera")
	void SetTargetBaseCar(ABaseCar* InCar);

	UFUNCTION(BlueprintCallable, Category = "Velocity Camera")
	void SetTargetPawn(APawn* InPawn);

	UFUNCTION(BlueprintPure, Category = "Velocity Camera")
	APawn* GetTargetPawn() const { return TargetPawn; }

	UFUNCTION(BlueprintCallable, Category = "Velocity Camera")
	void ResetCamera();

	// --- Camera Shake Triggers ---
	UFUNCTION(BlueprintCallable, Category = "Velocity Camera|Shake")
	void TriggerCollisionShake(float ImpactMagnitude = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Velocity Camera|Shake")
	void TriggerCrashShake(float Intensity = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Velocity Camera|Shake")
	void TriggerHighSpeedWindShake(float WindIntensity = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Velocity Camera|Shake")
	void TriggerEngineRedlineShake(float RPMPercent = 1.0f);

	// --- Dynamic FOV Hooks ---
	UFUNCTION(BlueprintPure, Category = "Velocity Camera|FOV")
	float GetDynamicFOV() const { return SmoothedFOV; }

	UFUNCTION(BlueprintCallable, Category = "Velocity Camera|FOV")
	void SetSpeedFOVRange(float InMinFOV, float InMaxFOV, float InMaxSpeedMPH);

	// --- Mode Presets ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Presets")
	FVelocityCameraPreset ChaseClosePreset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Presets")
	FVelocityCameraPreset ChaseFarPreset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Presets")
	FVelocityCameraPreset HoodPreset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Presets")
	FVelocityCameraPreset CockpitPreset;

	// --- Blending Settings ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Blending")
	float BlendDuration;

	UPROPERTY(BlueprintReadOnly, Category = "Camera|Blending")
	bool bIsBlending;

	UPROPERTY(BlueprintReadOnly, Category = "Camera|Blending")
	float BlendAlpha;

	// --- Speed FOV & Motion Blur ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Speed Effects")
	float BaseMinFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Speed Effects")
	float MaxSpeedFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Speed Effects")
	float ReferenceMaxSpeedMPH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Speed Effects")
	float FOVInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Speed Effects")
	float MinMotionBlurAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Speed Effects")
	float MaxMotionBlurAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Speed Effects")
	bool bEnableMotionBlurEffect;

	// --- Collision Settings ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Collision")
	float CollisionProbeRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Collision")
	float CollisionRecoverySpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Collision")
	TEnumAsByte<ECollisionChannel> CameraCollisionChannel;

	// --- High Speed Camera Shake ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Shake")
	bool bEnableHighSpeedShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Shake")
	float ShakeSpeedThresholdMPH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Shake")
	float MaxShakePitchYawRoll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Shake")
	float ShakeFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Shake")
	TSubclassOf<UCameraShakeBase> ImpactShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Shake")
	TSubclassOf<UCameraShakeBase> HighSpeedShakeClass;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Camera|State")
	EVelocityCameraMode CurrentCameraMode;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Camera|State")
	EVelocityCameraMode PreviousCameraMode;

	UPROPERTY()
	TObjectPtr<APawn> TargetPawn;

	UPROPERTY()
	TObjectPtr<AVELOCITYXVehicle> TargetVelocityVehicle;

	UPROPERTY()
	TObjectPtr<ABaseCar> TargetBaseCar;

	// Smoothed runtime state
	FVector SmoothedCameraLocation;
	FRotator SmoothedCameraRotation;
	float SmoothedFOV;

	// Blend start snapshots
	FVector BlendStartLocation;
	FRotator BlendStartRotation;
	float BlendStartFOV;
	float BlendTimer;

	// Collision state
	float CurrentCollisionDistanceAlpha;

	// Shake accumulation
	float ShakeTimer;
	float ExternalWindShakeIntensity;
	float ExternalRedlineShakeIntensity;

	// Helpers
	const FVelocityCameraPreset& GetPresetForMode(EVelocityCameraMode Mode) const;
	void ComputeModeTransform(EVelocityCameraMode Mode, const APawn* Pawn, FVector& OutLocation, FRotator& OutRotation, float& OutFOV) const;
	FVector SolveCameraCollision(const FVector& LookAtPoint, const FVector& DesiredLocation, float DeltaTime);
	void ApplySpeedEffects(float SpeedMPH, FTViewTarget& OutVT, float DeltaTime);
	void ApplyCameraShake(float SpeedMPH, float DeltaTime, FRotator& InOutRotation, FVector& InOutLocation);
	float ResolveVehicleSpeedMPH() const;
};

// Teammate Class Name Alias
typedef AVelocityCameraManager AVELOCITYXCameraManager;
