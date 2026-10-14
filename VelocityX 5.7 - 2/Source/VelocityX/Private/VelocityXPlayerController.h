// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VelocityXCameraMode.h"
#include "GameFramework/PlayerController.h"
#include "VelocityXPlayerController.generated.h"

enum class EVelocityXCarType : uint8;

/** Available camera perspectives for vehicle driving */


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraModeChanged, EVelocityXCameraMode, NewCameraMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPauseMenuToggled, bool, bIsPaused);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHeadlightsToggled, bool, bAreHeadlightsOn);

/**
 * AVelocityXPlayerController
 * Comprehensive Player Controller for VELOCITY X:
 * - Driving controls: WASD / Arrow keys, Gamepad analog triggers & stick
 * - Actions: Space (Handbrake), C (Camera cycle), L (Headlights), R (Reset), Esc (Pause)
 * - Cockpit free mouse look (Turn & LookUp)
 * - Pre-race vehicle selection interface
 * - Universal interface routing to ABaseCar, AChaosVehicle, and custom Pawns
 */
UCLASS()
class VELOCITYX_API AVelocityXPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AVelocityXPlayerController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	// --- Vehicle Driving Inputs (WASD / Arrows) ---
	UFUNCTION(BlueprintCallable, Category = "VelocityX|Input")
	void InputThrottle(float Value);

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Input")
	void InputBrake(float Value);

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Input")
	void InputSteering(float Value);

	// --- Action Handlers ---
	UFUNCTION(BlueprintCallable, Category = "VelocityX|Input")
	void InputHandbrakePressed();

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Input")
	void InputHandbrakeReleased();

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Input")
	void ToggleCameraMode();

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Input")
	void SetCameraMode(EVelocityXCameraMode NewMode);

	UFUNCTION(BlueprintPure, Category = "VelocityX|Input")
	EVelocityXCameraMode GetCameraMode() const { return CurrentCameraMode; }

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Input")
	void ToggleHeadlights();

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Input")
	void ResetVehicleUpright();

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Input")
	void TogglePauseMenu();

	// --- Mouse Look for Cockpit View ---
	UFUNCTION(BlueprintCallable, Category = "VelocityX|Input")
	void InputTurn(float Value);

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Input")
	void InputLookUp(float Value);

	// --- Vehicle Selection (Before Gameplay) ---
	UFUNCTION(BlueprintCallable, Category = "VelocityX|VehicleSelection")
	void RequestCarSelection(EVelocityXCarType CarType);

	UFUNCTION(BlueprintCallable, Category = "VelocityX|VehicleSelection")
	void ConfirmCarSelectionAndStartRace();

	// --- UI & Input Mode Helpers ---
	UFUNCTION(BlueprintCallable, Category = "VelocityX|UI")
	void SetDrivingInputMode();

	UFUNCTION(BlueprintCallable, Category = "VelocityX|UI")
	void SetMenuInputMode();

	// --- State Getters ---
	UFUNCTION(BlueprintPure, Category = "VelocityX|State")
	float GetCurrentThrottle() const { return CurrentThrottle; }

	UFUNCTION(BlueprintPure, Category = "VelocityX|State")
	float GetCurrentBrake() const { return CurrentBrake; }

	UFUNCTION(BlueprintPure, Category = "VelocityX|State")
	float GetCurrentSteering() const { return CurrentSteering; }

	UFUNCTION(BlueprintPure, Category = "VelocityX|State")
	bool IsHandbrakeEngaged() const { return bHandbrakeActive; }

	UFUNCTION(BlueprintPure, Category = "VelocityX|State")
	bool AreHeadlightsActive() const { return bHeadlightsActive; }

	// --- Delegates ---
	UPROPERTY(BlueprintAssignable, Category = "VelocityX|Events")
	FOnCameraModeChanged OnCameraModeChanged;

	UPROPERTY(BlueprintAssignable, Category = "VelocityX|Events")
	FOnPauseMenuToggled OnPauseMenuToggled;

	UPROPERTY(BlueprintAssignable, Category = "VelocityX|Events")
	FOnHeadlightsToggled OnHeadlightsToggled;

protected:
	// Live input values
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VelocityX|State")
	float CurrentThrottle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VelocityX|State")
	float CurrentBrake;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VelocityX|State")
	float CurrentSteering;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VelocityX|State")
	bool bHandbrakeActive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VelocityX|State")
	bool bHeadlightsActive;

	// Camera state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|Camera")
	EVelocityXCameraMode CurrentCameraMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|Camera")
	FRotator CockpitLookOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|Camera")
	float MouseLookSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|Camera")
	float CockpitLookMaxYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|Camera")
	float CockpitLookMaxPitch;

	// Input smoothing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|Input")
	float SteeringInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|Input")
	float PedalInterpSpeed;

	float TargetThrottle;
	float TargetBrake;
	float TargetSteering;

	// Internal routing
	void RouteInputsToPossessedPawn(float DeltaTime);
	void ApplyCameraModeTransform();
	void SetupCameraComponentsForPawn(APawn* InPawn);

	// Socket names for cameras
	static const FName SocketChaseCamera;
	static const FName SocketHoodCamera;
	static const FName SocketCockpitCamera;
};
