// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "VelocityInputConfig.generated.h"

/**
 * Camera perspective modes available in VELOCITY X
 * Supports both standard presets and specific close/far chase views requested by game systems.
 */
UENUM(BlueprintType)
enum class EVelocityCameraMode : uint8
{
	ChaseClose		UMETA(DisplayName = "Chase Close (Behind Car)"),
	ChaseFar		UMETA(DisplayName = "Chase Far (Behind Car High)"),
	Hood			UMETA(DisplayName = "Hood Camera (Front Driver View)"),
	Cockpit			UMETA(DisplayName = "Cockpit Camera (Inside Cabin)"),
	Top				UMETA(DisplayName = "Top Camera (Bird's Eye View)")
};

// Aliases for compatibility
typedef EVelocityCameraMode ECameraMode;

/**
 * Input Action Names for UE5 Enhanced Input and Legacy Fallback
 */
struct FVelocityInputNames
{
	// Digital / Axis Actions
	static const FName Throttle;
	static const FName Brake;
	static const FName SteerLeft;
	static const FName SteerRight;
	static const FName Handbrake;
	static const FName CycleCamera;
	static const FName ToggleHeadlights;
	static const FName ResetCar;
	static const FName Pause;
	static const FName MouseSteer;
	static const FName ShiftUp;
	static const FName ShiftDown;

	// Enhanced Input Action asset default identifiers
	static const FName IA_Throttle;
	static const FName IA_Brake;
	static const FName IA_Steer;
	static const FName IA_Handbrake;
	static const FName IA_CycleCamera;
	static const FName IA_ToggleHeadlights;
	static const FName IA_ResetCar;
	static const FName IA_Pause;
	static const FName IA_MouseSteer;
	static const FName IA_LookAround;
	static const FName IA_ShiftUp;
	static const FName IA_ShiftDown;
};

/**
 * Data asset holding all Enhanced Input actions and mapping contexts for VELOCITY X
 */
UCLASS(BlueprintType, Const)
class VELOCITYX_API UVelocityInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UVelocityInputConfig();

	// --- Input Mapping Contexts ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|MappingContext")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|MappingContext")
	TObjectPtr<UInputMappingContext> DrivingMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|MappingContext")
	TObjectPtr<UInputMappingContext> MenuMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|MappingContext")
	TObjectPtr<UInputMappingContext> PauseMappingContext;

	// --- Vehicle Movement Actions ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Driving")
	TObjectPtr<UInputAction> IA_Throttle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Driving")
	TObjectPtr<UInputAction> IA_Brake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Driving")
	TObjectPtr<UInputAction> IA_Steer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Driving")
	TObjectPtr<UInputAction> IA_Handbrake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Driving")
	TObjectPtr<UInputAction> IA_ShiftUp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Driving")
	TObjectPtr<UInputAction> IA_ShiftDown;

	// --- Camera Actions ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Camera")
	TObjectPtr<UInputAction> IA_CycleCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Camera")
	TObjectPtr<UInputAction> IA_LookAround;

	// --- Vehicle & Game Utility Actions ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Vehicle")
	TObjectPtr<UInputAction> IA_ToggleHeadlights;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Vehicle")
	TObjectPtr<UInputAction> IA_ResetCar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|System")
	TObjectPtr<UInputAction> IA_Pause;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Assist")
	TObjectPtr<UInputAction> IA_MouseSteer;

	// --- Priority Settings ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Priority")
	int32 DrivingContextPriority = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Priority")
	int32 MenuContextPriority = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Priority")
	int32 PauseContextPriority = 20;
};
