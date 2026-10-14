// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VelocityPlayerController.generated.h"

class ABaseCar;

/**
 * VELOCITY X Player Controller
 * Handles player input mapping, vehicle possession, and camera control.
 */
UCLASS()
class VELOCITYX_API AVelocityPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AVelocityPlayerController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// --- Vehicle Driving Inputs ---
	UFUNCTION(BlueprintCallable, Category = "Vehicle Input")
	void SetThrottleInput(float Value);

	UFUNCTION(BlueprintCallable, Category = "Vehicle Input")
	void SetSteeringInput(float Value);

	UFUNCTION(BlueprintCallable, Category = "Vehicle Input")
	void SetBrakeInput(float Value);

	UFUNCTION(BlueprintCallable, Category = "Vehicle Input")
	void SetHandbrakeInput(bool bPressed);

	// --- Action Handlers (Legacy & Enhanced Input Bindings) ---
	UFUNCTION(BlueprintCallable, Category = "Vehicle Input")
	void InputThrottleAxis(float Value);

	UFUNCTION(BlueprintCallable, Category = "Vehicle Input")
	void InputSteeringAxis(float Value);

	UFUNCTION(BlueprintCallable, Category = "Vehicle Input")
	void InputBrakeAxis(float Value);

	UFUNCTION(BlueprintCallable, Category = "Vehicle Actions")
	void ActionHandbrakePressed();

	UFUNCTION(BlueprintCallable, Category = "Vehicle Actions")
	void ActionHandbrakeReleased();

	UFUNCTION(BlueprintCallable, Category = "Vehicle Actions")
	void ActionResetCar();

	UFUNCTION(BlueprintCallable, Category = "Vehicle Actions")
	void ActionToggleHeadlights();

	UFUNCTION(BlueprintCallable, Category = "Vehicle Actions")
	void ActionCycleCamera();

	UFUNCTION(BlueprintCallable, Category = "Vehicle Actions")
	void ActionTogglePause();

	// --- Vehicle Accessor ---
	UFUNCTION(BlueprintPure, Category = "Vehicle")
	ABaseCar* GetControlledCar() const { return ControlledCar; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle")
	ABaseCar* ControlledCar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input State")
	float ThrottleValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input State")
	float SteeringValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input State")
	float BrakeValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input State")
	bool bHandbrakeActive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	bool bIsGamePaused;

	void UpdateControlledVehicleInput();
};
