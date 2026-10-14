// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#if __has_include("BaseCar.h")
#include "BaseCar.h"
#include "Supercar.h"
#include "TrackSupercar.h"
#include "Hypercar.h"
#elif __has_include("Car/BaseCar.h")
#include "Car/BaseCar.h"
#include "Car/Supercar.h"
#include "Car/TrackSupercar.h"
#include "Car/Hypercar.h"
#endif

#if __has_include("VELOCITYXGameMode.h")
#include "VELOCITYXGameMode.h"
#elif __has_include("../VELOCITYX/Public/VELOCITYXGameMode.h")
#include "../VELOCITYX/Public/VELOCITYXGameMode.h"
#endif

#include "CarManager.generated.h"

class APlayerController;

/** Performance benchmark test run results */
USTRUCT(BlueprintType)
struct FVehicleBenchmarkResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Benchmark")
	FString VehicleName = TEXT("");

	UPROPERTY(BlueprintReadOnly, Category = "Benchmark")
	float Time0To100Kmh = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Benchmark")
	float Time0To200Kmh = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Benchmark")
	float MaxSpeedAchievedKmh = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Benchmark")
	float BrakingDistance100To0Meters = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Benchmark")
	float PeakLateralG = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Benchmark")
	bool bTestCompleted = false;
};

/** Summary performance stats for UI car selection screen */
USTRUCT(BlueprintType)
struct FVehicleDisplayStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FString VehicleName = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float TopSpeedKmh = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Acceleration0To100Seconds = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Horsepower = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float WeightKg = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float HandlingRating = 80.0f; // 0 - 100 rating for UI bars

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BrakingRating = 85.0f;  // 0 - 100 rating for UI bars
};

/**
 * ACarManager
 * Central vehicle lifecycle and physics manager for VELOCITY X.
 * Handles spawning, switching between car variants (Supercar, Track Supercar, Hypercar),
 * global environmental physics settings, and real-time performance telemetry benchmarks.
 */
UCLASS()
class VELOCITYX_API ACarManager : public AActor
{
	GENERATED_BODY()

public:
	ACarManager();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// --- Vehicle Spawning & Switching ---

	/** Spawn vehicle of chosen type at specified transform */
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|CarManager")
	ABaseCar* SpawnVehicle(TSubclassOf<ABaseCar> VehicleClass, const FTransform& SpawnTransform);

	/** Spawn vehicle directly by ECarType enum matching VELOCITYXGameMode */
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|CarManager")
	ABaseCar* SpawnVehicleByType(ECarType Type, const FTransform& Transform);

	/** Switch active player car by type name: "Supercar", "TrackSupercar", "Hypercar" */
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|CarManager")
	ABaseCar* SwitchActiveVehicle(const FString& VehicleType, APlayerController* PC);

	/** Switch active vehicle by index: 0 = Supercar, 1 = Track Supercar, 2 = Hypercar */
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|CarManager")
	ABaseCar* SwitchActiveVehicleByIndex(int32 VehicleIndex, APlayerController* PC);

	/** Switch active vehicle by ECarType enum */
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|CarManager")
	ABaseCar* SwitchActiveVehicleByType(ECarType Type, APlayerController* PC);

	/** Get car display stats for UI car selection screen */
	UFUNCTION(BlueprintPure, Category = "VELOCITY X|CarManager")
	FVehicleDisplayStats GetCarStats(ECarType Type) const;

	/** Get the currently active possessed vehicle */
	UFUNCTION(BlueprintPure, Category = "VELOCITY X|CarManager")
	ABaseCar* GetActiveVehicle() const { return ActiveVehicle; }

	// --- Vehicle Classes Registered ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|CarManager|Classes")
	TSubclassOf<ASupercar> SupercarClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|CarManager|Classes")
	TSubclassOf<ATrackSupercar> TrackSupercarClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|CarManager|Classes")
	TSubclassOf<AHypercar> HypercarClass;

	// --- Global Environment Settings ---

	/** Set track surface condition across the simulation */
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Environment")
	void SetGlobalSurfaceType(ESurfaceType NewSurface);

	/** Get current global surface type */
	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Environment")
	ESurfaceType GetGlobalSurfaceType() const { return GlobalSurfaceType; }

	/** Set ambient air density for aerodynamic drag calculations */
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Environment")
	void SetAirDensity(float DensityKgM3);

	// --- Performance Benchmarking ---

	/** Start an automated 0-100 km/h acceleration benchmark run */
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Benchmark")
	void StartBenchmarkTest();

	/** Reset benchmark data */
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Benchmark")
	void ResetBenchmarkTest();

	/** Get latest benchmark test results */
	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Benchmark")
	FVehicleBenchmarkResult GetBenchmarkResult() const { return BenchmarkResult; }

	/** Static helper to get CarManager in world */
	UFUNCTION(BlueprintPure, Category = "VELOCITY X|CarManager", meta = (WorldContext = "WorldContextObject"))
	static ACarManager* GetCarManager(const UObject* WorldContextObject);

protected:
	UPROPERTY()
	ABaseCar* ActiveVehicle = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Environment")
	ESurfaceType GlobalSurfaceType = ESurfaceType::DryAsphalt;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Environment")
	float GlobalAirDensity = 1.225f;

	// Benchmark state
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Benchmark")
	FVehicleBenchmarkResult BenchmarkResult;

	bool bBenchmarkRunning = false;
	float BenchmarkTimer = 0.0f;
	FVector BenchmarkStartLocation = FVector::ZeroVector;
	bool bPassed100Kmh = false;
	bool bPassed200Kmh = false;

	void UpdateBenchmark(float DeltaTime);
};
