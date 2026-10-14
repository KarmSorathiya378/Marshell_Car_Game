// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VelocityTelemetryTracker.generated.h"

/**
 * Real-time telemetry snapshot structure exposed to Blueprints and UI
 */
USTRUCT(BlueprintType)
struct FTelemetrySnapshot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float SpeedMPH = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float SpeedKPH = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float EngineRPM = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	int32 CurrentGear = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float CurrentLapTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float BestLapTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float LastLapTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	int32 CurrentLapNumber = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float TopSpeedMPH = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float TopSpeedKPH = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float MaxRPM = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float DistanceTraveledMeters = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float ThrottleInput = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float BrakeInput = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float SteeringInput = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float LateralGForce = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float LongitudinalGForce = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry")
	float Timestamp = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTelemetryUpdatedSignature, const FTelemetrySnapshot&, Snapshot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewBestLapSignature, float, NewBestLapTime, int32, LapNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLapCompletedSignature, float, LapTime, int32, LapNumber, bool, bWasBestLap);

/**
 * Velocity Telemetry Tracker
 * Real-time telemetry monitoring, lap timing, speed/RPM logging, and peak metric tracking.
 */
UCLASS(Blueprintable, BlueprintType)
class VELOCITYX_API UVelocityTelemetryTracker : public UObject
{
	GENERATED_BODY()

public:
	UVelocityTelemetryTracker();

	// --- Live Telemetry Update ---
	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void UpdateTelemetry(float InSpeedMPH, float InSpeedKPH, float InRPM, int32 InGear, float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void UpdateInputs(float InThrottle, float InBrake, float InSteering);

	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void UpdateGForces(float InLateralG, float InLongitudinalG);

	// --- Lap Tracking ---
	UFUNCTION(BlueprintCallable, Category = "Telemetry|Laps")
	void StartNewLap();

	UFUNCTION(BlueprintCallable, Category = "Telemetry|Laps")
	void CompleteCurrentLap();

	UFUNCTION(BlueprintCallable, Category = "Telemetry|Laps")
	void RecordLapTime(float LapTime);

	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void ResetTelemetry();

	// --- Getters ---
	UFUNCTION(BlueprintPure, Category = "Telemetry")
	FTelemetrySnapshot GetCurrentSnapshot() const { return CurrentSnapshot; }

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	float GetCurrentSpeedMPH() const { return CurrentSnapshot.SpeedMPH; }

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	float GetCurrentSpeedKPH() const { return CurrentSnapshot.SpeedKPH; }

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	float GetCurrentRPM() const { return CurrentSnapshot.EngineRPM; }

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	int32 GetCurrentGear() const { return CurrentSnapshot.CurrentGear; }

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	float GetCurrentLapTime() const { return CurrentSnapshot.CurrentLapTime; }

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	float GetBestLapTime() const { return CurrentSnapshot.BestLapTime; }

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	float GetLastLapTime() const { return CurrentSnapshot.LastLapTime; }

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	int32 GetCurrentLapNumber() const { return CurrentSnapshot.CurrentLapNumber; }

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	float GetTopSpeedMPH() const { return CurrentSnapshot.TopSpeedMPH; }

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	float GetTopSpeedKPH() const { return CurrentSnapshot.TopSpeedKPH; }

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	float GetMaxRPM() const { return CurrentSnapshot.MaxRPM; }

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	TArray<float> GetLapHistory() const { return LapHistory; }

	// --- Delegates ---
	UPROPERTY(BlueprintAssignable, Category = "Telemetry|Events")
	FOnTelemetryUpdatedSignature OnTelemetryUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Telemetry|Events")
	FOnNewBestLapSignature OnNewBestLap;

	UPROPERTY(BlueprintAssignable, Category = "Telemetry|Events")
	FOnLapCompletedSignature OnLapCompleted;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Telemetry")
	FTelemetrySnapshot CurrentSnapshot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Telemetry")
	TArray<float> LapHistory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Telemetry")
	bool bLapActive;
};
