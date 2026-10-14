// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AVelocityXChaosVehicle.h"
#include "AVelocityXCar.generated.h"

class USpotLightComponent;
class UPointLightComponent;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;

/** Preset configurations for vehicle classes */
UENUM(BlueprintType)
enum class EVelocityXCarPreset : uint8
{
	Supercar UMETA(DisplayName = "Supercar (Balanced, High Speed)"),
	TrackSupercar UMETA(DisplayName = "Track Supercar (High Downforce, Grip)"),
	Hypercar UMETA(DisplayName = "Hypercar (Extreme Top Speed, AWD)")
};

/** Car specifications and UI comparison ratings */
USTRUCT(BlueprintType)
struct FVelocityXCarStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Stats")
	FString CarName = TEXT("Supercar");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Stats")
	float MaxSpeedKmh = 320.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Stats")
	float Acceleration0To100Seconds = 3.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Stats")
	float HandlingRating = 88.0f; // 0 - 100 rating

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Stats")
	float BrakingRating = 86.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Stats")
	float Horsepower = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Stats")
	float PeakTorqueNm = 470.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Stats")
	float WeightKg = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Stats")
	float DownforceCoefficient = 1.85f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Stats")
	bool bIsAWD = false;
};

/**
 * AVelocityXCar
 * Complete playable car pawn for VELOCITY X.
 * Extends Chaos vehicle physics with:
 * - 3 distinct presets (Supercar, Track SuperCar, HyperCar) with realistic stats
 * - Dynamic headlight, taillight, and brake light systems
 * - Rotating and steering visual wheel mesh components
 * - Socket attachments for multi-mode camera transitions
 */
UCLASS()
class VELOCITYX_API AVelocityXCar : public AVelocityXChaosVehicle
{
	GENERATED_BODY()

public:
	AVelocityXCar();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// --- Preset Configurations ---
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Preset")
	void ApplyPreset(EVelocityXCarPreset InPreset);

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Preset")
	EVelocityXCarPreset GetCurrentPreset() const { return CurrentPreset; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Preset")
	FVelocityXCarStats GetCarStats() const { return ActiveStats; }

	// --- Lighting Controls ---
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Lighting")
	void ToggleHeadlights();

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Lighting")
	void SetHeadlights(bool bEnable);

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Lighting")
	bool AreHeadlightsOn() const { return bHeadlightsOn; }

	// --- Sockets for Cameras & Attachments ---
	static const FName SocketCockpitCamera;
	static const FName SocketHoodCamera;
	static const FName SocketChaseBoom;
	static const FName SocketExhaustLeft;
	static const FName SocketExhaustRight;

	/** Get location for a specific camera socket */
	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Camera")
	FVector GetCameraSocketLocation(FName SocketName) const;

	/** Get rotation for a specific camera socket */
	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Camera")
	FRotator GetCameraSocketRotation(FName SocketName) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Preset")
	EVelocityXCarPreset CurrentPreset = EVelocityXCarPreset::Supercar;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Preset")
	FVelocityXCarStats ActiveStats;

	// --- Lighting Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VELOCITY X|Lighting")
	USpotLightComponent* HeadlightLeft = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VELOCITY X|Lighting")
	USpotLightComponent* HeadlightRight = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VELOCITY X|Lighting")
	UPointLightComponent* TaillightLeft = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VELOCITY X|Lighting")
	UPointLightComponent* TaillightRight = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VELOCITY X|Lighting")
	UPointLightComponent* ReverseLight = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Lighting")
	bool bHeadlightsOn = false;

	// --- Rotating Visual Wheel Components (0: FL, 1: FR, 2: RL, 3: RR) ---
	UPROPERTY(VisibleAnywhere, Category = "VELOCITY X|Visuals")
	UStaticMeshComponent* WheelMeshComponents[4];

	// Sub-routines
	virtual void UpdateVisualWheels(float DeltaTime);
	virtual void UpdateLighting(float DeltaTime);
};
