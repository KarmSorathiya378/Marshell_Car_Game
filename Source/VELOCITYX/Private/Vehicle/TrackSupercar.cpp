// Copyright VELOCITY X Racing. All Rights Reserved.

#include "Vehicle/TrackSupercar.h"

ATrackSupercar::ATrackSupercar()
{
	bDRSActive = false;
	SetupCarDefaults();
}

void ATrackSupercar::SetupCarDefaults()
{
	Super::SetupCarDefaults();

	PerformanceStats.ModelName = TEXT("Vortex R Track Spec");
	PerformanceStats.Horsepower = 780.0f;
	PerformanceStats.WeightKg = 1240.0f; // Lightweight track carbon chassis
	PerformanceStats.MaxSpeedMPH = 215.0f;
	PerformanceStats.Acceleration0to60 = 2.5f;
	PerformanceStats.BrakeForce = 6200.0f; // Carbon ceramic race brakes
	PerformanceStats.SteerSensitivity = 1.25f;
	PerformanceStats.GripFactor = 1.35f; // Racing slicks
	PerformanceStats.DragCoefficient = 0.38f;
	PerformanceStats.DownforceMultiplier = 2.4f; // Extreme aero wing & diffuser
	PerformanceStats.NumGears = 6; // Sequential racing gearbox
	PerformanceStats.EngineTorque = 820.0f;
	PerformanceStats.RedlineRPM = 9200.0f; // High-revving race engine

	BaseDownforce = PerformanceStats.DownforceMultiplier;
	BaseDrag = PerformanceStats.DragCoefficient;
}

void ATrackSupercar::SetDRSActive(bool bActive)
{
	if (bDRSActive == bActive)
	{
		return;
	}

	bDRSActive = bActive;
	if (bDRSActive)
	{
		// Reduce drag and downforce for straight-line speed
		PerformanceStats.DragCoefficient = BaseDrag * 0.70f;
		PerformanceStats.DownforceMultiplier = BaseDownforce * 0.65f;
		UE_LOG(LogTemp, Display, TEXT("Vortex R: DRS OPEN. Drag reduced for maximum straight-line speed."));
	}
	else
	{
		PerformanceStats.DragCoefficient = BaseDrag;
		PerformanceStats.DownforceMultiplier = BaseDownforce;
		UE_LOG(LogTemp, Display, TEXT("Vortex R: DRS CLOSED. Maximum cornering downforce restored."));
	}
}
