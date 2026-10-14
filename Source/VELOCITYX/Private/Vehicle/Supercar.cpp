// Copyright VELOCITY X Racing. All Rights Reserved.

#include "Vehicle/Supercar.h"

ASupercar::ASupercar()
{
	bSportMode = false;
	SportModeThrottleMultiplier = 1.15f;
	SetupCarDefaults();
}

void ASupercar::SetupCarDefaults()
{
	Super::SetupCarDefaults();

	PerformanceStats.ModelName = TEXT("Apex GT Supercar");
	PerformanceStats.Horsepower = 670.0f;
	PerformanceStats.WeightKg = 1420.0f;
	PerformanceStats.MaxSpeedMPH = 208.0f;
	PerformanceStats.Acceleration0to60 = 2.9f;
	PerformanceStats.BrakeForce = 4500.0f;
	PerformanceStats.SteerSensitivity = 1.05f;
	PerformanceStats.GripFactor = 1.1f;
	PerformanceStats.DragCoefficient = 0.30f;
	PerformanceStats.DownforceMultiplier = 1.2f;
	PerformanceStats.NumGears = 7;
	PerformanceStats.EngineTorque = 760.0f;
	PerformanceStats.RedlineRPM = 8200.0f;
}

void ASupercar::ToggleSportMode()
{
	bSportMode = !bSportMode;
	if (bSportMode)
	{
		PerformanceStats.SteerSensitivity *= 1.1f;
		UE_LOG(LogTemp, Display, TEXT("Apex GT: Sport Mode ENGAGED. Throttle response sharpened."));
	}
	else
	{
		PerformanceStats.SteerSensitivity /= 1.1f;
		UE_LOG(LogTemp, Display, TEXT("Apex GT: Sport Mode DISENGAGED. Comfort profile active."));
	}
}
