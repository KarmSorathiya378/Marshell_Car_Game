// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VELOCITYXPlayerController.h"
#include "VelocityInputConfig.h"
#include "InputActionValue.h"
#include "VelocityPlayerController.generated.h"

class AVelocityCameraManager;
class AVELOCITYXVehicle;
class ABaseCar;
class UEnhancedInputComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVelocityPauseToggled, bool, bIsPaused);

/**
 * Enhanced Player Controller for VELOCITY X
 * Supports:
 * - Direct routing to Zelda's ABaseCar and Chaos vehicles (Throttle, Brake, Steer, Handbrake, Shifting, Reset, Headlights)
 * - Input Context Switching: IMC_Driving, IMC_Menu, IMC_Pause
 * - Gamepad analog trigger processing with gamma linearity, sensitivity, and deadzones
 * - Dedicated haptic feedback hooks (Tire slip, Redline, ABS pulse, Collision shock)
 * - 4-Mode Camera Manager cycling and socket integration
 * - Settings integration for UVELOCITYXGameInstance / UI
 */
UCLASS()
class VELOCITYX_API AVelocityPlayerController : public AVELOCITYXPlayerController
{
	GENERATED_BODY()

public:
	AVelocityPlayerController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	// --- Overridden Player Actions ---
	virtual void CycleCamera();
	virtual void SetCamera(int Index);
	virtual void TogglePause();
	virtual void ResetCar();
	virtual void ToggleHeadlights();

	// --- Manual Shifting ---
	UFUNCTION(BlueprintCallable, Category = "Vehicle Controls|Transmission")
	void ShiftUp();

	UFUNCTION(BlueprintCallable, Category = "Vehicle Controls|Transmission")
	void ShiftDown();

	// --- Input Context Management (IMC Switching) ---
	UFUNCTION(BlueprintCallable, Category = "Velocity Input|Context")
	void SetInputModeDriving();

	UFUNCTION(BlueprintCallable, Category = "Velocity Input|Context")
	void SetInputModeUI();

	UFUNCTION(BlueprintCallable, Category = "Velocity Input|Context")
	void SetInputModePause();

	// Pause delegate
	UPROPERTY(BlueprintAssignable, Category = "Velocity Game|Pause")
	FOnVelocityPauseToggled OnPauseToggled;

	// --- Enhanced Input Action Handlers ---
	UFUNCTION(BlueprintCallable, Category = "Velocity Input")
	void HandleEnhancedThrottle(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Velocity Input")
	void HandleEnhancedThrottleCompleted(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Velocity Input")
	void HandleEnhancedBrake(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Velocity Input")
	void HandleEnhancedBrakeCompleted(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Velocity Input")
	void HandleEnhancedSteer(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Velocity Input")
	void HandleEnhancedSteerCompleted(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Velocity Input")
	void HandleEnhancedHandbrake(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Velocity Input")
	void HandleEnhancedHandbrakeCompleted(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Velocity Input")
	void HandleEnhancedMouseSteer(const FInputActionValue& Value);

	// --- Analog Gamepad Handlers ---
	UFUNCTION(BlueprintCallable, Category = "Velocity Gamepad")
	void HandleGamepadAnalogThrottle(float Value);

	UFUNCTION(BlueprintCallable, Category = "Velocity Gamepad")
	void HandleGamepadAnalogBrake(float Value);

	UFUNCTION(BlueprintCallable, Category = "Velocity Gamepad")
	void HandleGamepadAnalogSteer(float Value);

	UFUNCTION(BlueprintCallable, Category = "Velocity Gamepad")
	void HandleGamepadQuickCameraChase();

	UFUNCTION(BlueprintCallable, Category = "Velocity Gamepad")
	void HandleGamepadQuickCameraCockpit();

	// --- Gamepad Haptics & Vibration Hooks ---
	UFUNCTION(BlueprintCallable, Category = "Velocity Haptics")
	void PlayHandbrakeHaptics(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Velocity Haptics")
	void PlayTireSlipHaptics(float SlipRatio);

	UFUNCTION(BlueprintCallable, Category = "Velocity Haptics")
	void PlayRedlineHaptics(float RPMNormalized);

	UFUNCTION(BlueprintCallable, Category = "Velocity Haptics")
	void PlayHardBrakingABSHaptics(float BrakeIntensity);

	UFUNCTION(BlueprintCallable, Category = "Velocity Haptics")
	void PlayCollisionHaptics(float ImpactForce);

	UFUNCTION(BlueprintCallable, Category = "Velocity Haptics")
	void TriggerCrashHaptics(float Intensity = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Velocity Haptics")
	void StopAllHaptics();

	// --- Settings & Config Integration ---
	UFUNCTION(BlueprintCallable, Category = "Velocity Settings")
	void ApplyControlSettings(float InSteerSens, float InSteerDeadzone, float InSteerLinearity, float InTriggerDeadzone);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocity Settings|Steering")
	float SteeringSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocity Settings|Steering")
	float SteeringDeadzone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocity Settings|Steering")
	float SteeringLinearity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocity Settings|Pedals")
	float TriggerDeadzone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocity Settings|Pedals")
	float ThrottleSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocity Settings|Pedals")
	float BrakeSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocity Settings|Haptics")
	bool bEnableGamepadVibration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocity Settings|Haptics")
	float HandbrakeRumbleIntensity;

	// Mouse steering assist
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocity Settings|Mouse Assist")
	bool bEnableMouseSteeringAssist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocity Settings|Mouse Assist")
	float MouseSteeringSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Velocity Settings|Mouse Assist")
	float MouseSteeringAutoCenterRate;

	// Config asset
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Config")
	TObjectPtr<UVelocityInputConfig> InputConfig;

	// Camera Manager reference
	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	TObjectPtr<AVelocityCameraManager> VelocityCameraManager;

protected:
	UPROPERTY()
	TObjectPtr<ABaseCar> TargetBaseCar;

	// Raw analog values
	float RawGamepadThrottle;
	float RawGamepadBrake;
	float RawGamepadSteer;
	float MouseSteerAccumulator;

	// Vibration timers
	bool bIsHandbrakeRumbling;
	float ABSPulseTimer;

	// Helpers
	void BindEnhancedInputActions(UEnhancedInputComponent* EnhancedInputComp);
	void ProcessVehicleInputs(float DeltaTime);
	void UpdateCameraManagerReference();
	void ResolveTargetVehicles();
	float ApplySteeringCurve(float RawInput) const;
};
