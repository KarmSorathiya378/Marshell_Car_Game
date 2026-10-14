// Copyright VELOCITY X Racing. All Rights Reserved.

#include "Vehicle/Hypercar.h"

AHypercar::AHypercar()
{
	bBoostActive = false;
	BoostCharge = 100.0f;
	MaxBoostCharge = 100.0f;
	BoostDepletionRate = 25.0f; // 4 seconds of full boost
	BoostRechargeRate = 10.0f;  // Regenerative braking / coasting recharge

	SetupCarDefaults();
}

void AHypercar::SetupCarDefaults()
{
	Super::SetupCarDefaults();

	PerformanceStats.ModelName = TEXT("Phantom X Hypercar");
	PerformanceStats.Horsepower = 1600.0f;
	PerformanceStats.WeightKg = 1580.0f;
	PerformanceStats.MaxSpeedMPH = 285.0f;
	PerformanceStats.Acceleration0to60 = 2.1f;
	PerformanceStats.BrakeForce = 6800.0f;
	PerformanceStats.SteerSensitivity = 1.15f;
	PerformanceStats.GripFactor = 1.25f;
	PerformanceStats.DragCoefficient = 0.28f; // Low drag active aero
	PerformanceStats.DownforceMultiplier = 1.8f;
	PerformanceStats.NumGears = 8;
	PerformanceStats.EngineTorque = 1500.0f;
	PerformanceStats.RedlineRPM = 9800.0f;

	BaseTorque = PerformanceStats.EngineTorque;
}

void AHypercar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle boost depletion and regen
	if (bBoostActive)
	{
		BoostCharge = FMath::Max(0.0f, BoostCharge - (BoostDepletionRate * DeltaTime));
		if (BoostCharge <= 0.0f)
		{
			SetHybridBoostActive(false);
		}
	}
	else
	{
		BoostCharge = FMath::Min(MaxBoostCharge, BoostCharge + (BoostRechargeRate * DeltaTime));
	}
}

void AHypercar::SetHybridBoostActive(bool bActive)
{
	if (bActive && BoostCharge > 5.0f)
	{
		bBoostActive = true;
		PerformanceStats.EngineTorque = BaseTorque * 1.35f; // +35% hybrid torque boost
		UE_LOG(LogTemp, Display, TEXT("Phantom X: HYBRID TORQUE BOOST ENGAGED!"));
	}
	else
	{
		bBoostActive = false;
		PerformanceStats.EngineTorque = BaseTorque;
	}
}
