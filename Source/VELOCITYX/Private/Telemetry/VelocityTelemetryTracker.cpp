// Copyright VELOCITY X Racing. All Rights Reserved.

#include "Telemetry/VelocityTelemetryTracker.h"

UVelocityTelemetryTracker::UVelocityTelemetryTracker()
{
	bLapActive = false;
	ResetTelemetry();
}

void UVelocityTelemetryTracker::ResetTelemetry()
{
	CurrentSnapshot = FTelemetrySnapshot();
	CurrentSnapshot.CurrentLapNumber = 1;
	LapHistory.Empty();
	bLapActive = true;
}

void UVelocityTelemetryTracker::UpdateTelemetry(float InSpeedMPH, float InSpeedKPH, float InRPM, int32 InGear, float DeltaTime)
{
	CurrentSnapshot.SpeedMPH = InSpeedMPH;
	CurrentSnapshot.SpeedKPH = InSpeedKPH;
	CurrentSnapshot.EngineRPM = InRPM;
	CurrentSnapshot.CurrentGear = InGear;
	CurrentSnapshot.Timestamp += DeltaTime;

	// Update peak metrics
	if (InSpeedMPH > CurrentSnapshot.TopSpeedMPH)
	{
		CurrentSnapshot.TopSpeedMPH = InSpeedMPH;
	}
	if (InSpeedKPH > CurrentSnapshot.TopSpeedKPH)
	{
		CurrentSnapshot.TopSpeedKPH = InSpeedKPH;
	}
	if (InRPM > CurrentSnapshot.MaxRPM)
	{
		CurrentSnapshot.MaxRPM = InRPM;
	}

	// Update distance traveled (1 KPH = 0.27778 m/s)
	float SpeedMps = InSpeedKPH * 0.277778f;
	CurrentSnapshot.DistanceTraveledMeters += (SpeedMps * DeltaTime);

	// Update current lap stopwatch if active
	if (bLapActive)
	{
		CurrentSnapshot.CurrentLapTime += DeltaTime;
	}

	// Broadcast snapshot update to UI and Blueprints
	OnTelemetryUpdated.Broadcast(CurrentSnapshot);
}

void UVelocityTelemetryTracker::UpdateInputs(float InThrottle, float InBrake, float InSteering)
{
	CurrentSnapshot.ThrottleInput = InThrottle;
	CurrentSnapshot.BrakeInput = InBrake;
	CurrentSnapshot.SteeringInput = InSteering;
}

void UVelocityTelemetryTracker::UpdateGForces(float InLateralG, float InLongitudinalG)
{
	CurrentSnapshot.LateralGForce = InLateralG;
	CurrentSnapshot.LongitudinalGForce = InLongitudinalG;
}

void UVelocityTelemetryTracker::StartNewLap()
{
	bLapActive = true;
	CurrentSnapshot.CurrentLapTime = 0.0f;
}

void UVelocityTelemetryTracker::CompleteCurrentLap()
{
	if (!bLapActive || CurrentSnapshot.CurrentLapTime <= 0.01f)
	{
		return;
	}

	RecordLapTime(CurrentSnapshot.CurrentLapTime);
	StartNewLap();
}

void UVelocityTelemetryTracker::RecordLapTime(float LapTime)
{
	CurrentSnapshot.LastLapTime = LapTime;
	LapHistory.Add(LapTime);

	bool bIsNewBest = false;
	if (CurrentSnapshot.BestLapTime <= 0.001f || LapTime < CurrentSnapshot.BestLapTime)
	{
		CurrentSnapshot.BestLapTime = LapTime;
		bIsNewBest = true;
		OnNewBestLap.Broadcast(LapTime, CurrentSnapshot.CurrentLapNumber);
		UE_LOG(LogTemp, Display, TEXT("NEW BEST LAP! Lap %d: %.3f seconds"), CurrentSnapshot.CurrentLapNumber, LapTime);
	}

	OnLapCompleted.Broadcast(LapTime, CurrentSnapshot.CurrentLapNumber, bIsNewBest);
	CurrentSnapshot.CurrentLapNumber++;
}
