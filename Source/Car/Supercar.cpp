// Copyright VELOCITY X Racing. All Rights Reserved.

#include "Supercar.h"

ASupercar::ASupercar()
{
	// Vehicle Identity
	PhysicsConfig.VehicleName = TEXT("Apex GT3-R (Supercar)");
	PhysicsConfig.TotalMassKg = 1500.0f;
	PhysicsConfig.WeightDistributionFront = 0.39f; // 39% front / 61% rear weight distribution
	PhysicsConfig.WheelbaseCm = 245.7f;
	PhysicsConfig.TrackWidthCm = 168.0f;
	PhysicsConfig.CenterOfMassHeightCm = 37.0f;
	PhysicsConfig.MaxSteeringAngle = 36.0f;
	PhysicsConfig.RatedTopSpeedKmh = 320.0f;
	PhysicsConfig.Rated0To100TimeSeconds = 3.5f;

	// Engine Parameters: 500 HP, 9000 RPM naturally aspirated flat-6
	PhysicsConfig.Engine.MaxHorsepower = 500.0f;
	PhysicsConfig.Engine.MaxTorque = 470.0f; // N*m @ 6300 RPM
	PhysicsConfig.Engine.IdleRPM = 900.0f;
	PhysicsConfig.Engine.RedlineRPM = 9000.0f;
	PhysicsConfig.Engine.EngineMomentOfInertia = 0.22f; // Ultra-responsive lightweight flywheel
	PhysicsConfig.Engine.EngineBrakingRate = 0.24f;     // High compression engine braking
	PhysicsConfig.Engine.RevLimiterCutDuration = 0.06f;
	PhysicsConfig.Engine.PowerToWeightRatio = 333.3f;   // 500 HP / 1.5 tonnes

	// Transmission Parameters: 7-Speed Dual-Clutch (PDK style)
	PhysicsConfig.Transmission.TransmissionMode = ETransmissionMode::Automatic;
	PhysicsConfig.Transmission.DriveType = EDriveType::RWD;
	// 1st to 7th gear ratios tuned for high-revving engine
	PhysicsConfig.Transmission.ForwardGearRatios = { 3.75f, 2.38f, 1.72f, 1.34f, 1.11f, 0.96f, 0.84f };
	PhysicsConfig.Transmission.ReverseGearRatio = 3.42f;
	PhysicsConfig.Transmission.FinalDriveRatio = 3.97f;
	PhysicsConfig.Transmission.GearChangeTime = 0.07f; // Lightning fast 70ms PDK shifts
	PhysicsConfig.Transmission.AutoUpshiftRPMPercent = 0.96f; // Upshifts near 8640 RPM
	PhysicsConfig.Transmission.AutoDownshiftRPMPercent = 0.38f;
	PhysicsConfig.Transmission.DifferentialType = EDifferentialType::LimitedSlip;
	PhysicsConfig.Transmission.DifferentialLockRatio = 0.55f; // Mechanical clutch LSD with balanced lockup
	PhysicsConfig.Transmission.FrontTorqueSplit = 0.0f; // 100% RWD

	// Suspension Parameters: Stiff track-oriented MacPherson front / Multi-link rear
	PhysicsConfig.Suspension.SpringRateFront = 85000.0f;   // 85 kN/m
	PhysicsConfig.Suspension.SpringRateRear = 120000.0f;   // 120 kN/m (supporting rear engine weight)
	PhysicsConfig.Suspension.DampingCompressionFront = 4800.0f;
	PhysicsConfig.Suspension.DampingCompressionRear = 6200.0f;
	PhysicsConfig.Suspension.DampingReboundFront = 7200.0f;
	PhysicsConfig.Suspension.DampingReboundRear = 9400.0f;
	PhysicsConfig.Suspension.RideHeight = 10.5f;          // 10.5 cm ground clearance
	PhysicsConfig.Suspension.SuspensionMaxRaise = 7.5f;
	PhysicsConfig.Suspension.SuspensionMaxDrop = 7.5f;
	PhysicsConfig.Suspension.AntiRollBarStiffnessFront = 16000.0f;
	PhysicsConfig.Suspension.AntiRollBarStiffnessRear = 22000.0f; // Stiffer rear sway bar encourages rotation
	PhysicsConfig.Suspension.CamberFront = -2.5f;          // Aggressive front negative camber
	PhysicsConfig.Suspension.CamberRear = -2.0f;

	// Tire Parameters: Ultra-high performance track tires (e.g. Michelin Cup 2 R)
	PhysicsConfig.Tires.FrictionMultiplier = 1.68f;        // High mechanical grip
	PhysicsConfig.Tires.CorneringStiffness = 19.5f;
	PhysicsConfig.Tires.LongitudinalStiffness = 23.0f;
	PhysicsConfig.Tires.RollingResistance = 0.013f;
	PhysicsConfig.Tires.TireWidthFront = 275.0f;           // 275mm front
	PhysicsConfig.Tires.TireWidthRear = 335.0f;            // 335mm massive rear rubber
	PhysicsConfig.Tires.WheelRadiusFront = 34.0f;
	PhysicsConfig.Tires.WheelRadiusRear = 36.0f;
	PhysicsConfig.Tires.OptimalSlipAngle = 8.0f;
	PhysicsConfig.Tires.OptimalSlipRatio = 0.12f;

	// Aerodynamic Properties: Swan-neck wing with active DRS
	PhysicsConfig.Aerodynamics.DragCoefficient = 0.38f;
	PhysicsConfig.Aerodynamics.FrontalArea = 2.12f;
	PhysicsConfig.Aerodynamics.DownforceCoefficient = 1.85f; // 860 kg total downforce at peak velocity
	PhysicsConfig.Aerodynamics.AeroBalanceFront = 0.38f;
	PhysicsConfig.Aerodynamics.AirDensity = 1.225f;
	PhysicsConfig.Aerodynamics.bHasActiveAero = true;
	PhysicsConfig.Aerodynamics.AirbrakeDragMultiplier = 2.2f;
	PhysicsConfig.Aerodynamics.AirbrakeDownforceMultiplier = 1.5f;

	// Active Driver Assistance: Calibrated for track drivers
	PhysicsConfig.Assistance.bABSEnabled = true;
	PhysicsConfig.Assistance.ABSSlipThreshold = 0.15f;
	PhysicsConfig.Assistance.bTCSEnabled = true;
	PhysicsConfig.Assistance.TCSSlipThreshold = 0.13f; // Permits moderate wheelspin before intervention
	PhysicsConfig.Assistance.TCSAggressiveness = 0.65f; // Progressive, non-intrusive cut
	PhysicsConfig.Assistance.bESPEnabled = true;
	PhysicsConfig.Assistance.MaxBrakeForce = 18500.0f;  // Carbon ceramic brake clamping force
	PhysicsConfig.Assistance.MaxHandbrakeForce = 11000.0f;
	PhysicsConfig.Assistance.BrakeBiasFront = 0.61f;

	bDRSOpen = false;
}

void ASupercar::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("[VELOCITY X] Supercar initialized with 500 HP / 1500 kg setup."));
}

void ASupercar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Custom handling dynamics tailored to Supercar (moderate oversteer on throttle lift/stab)
	SimulateSupercarHandling(DeltaTime);
}

void ASupercar::ToggleDRS()
{
	bDRSOpen = !bDRSOpen;

	if (bDRSOpen)
	{
		// Shed 30% drag and downforce for high-speed straightaways
		PhysicsConfig.Aerodynamics.DragCoefficient = 0.28f;
		PhysicsConfig.Aerodynamics.DownforceCoefficient = 0.90f;
		UE_LOG(LogTemp, Display, TEXT("[Supercar] DRS OPEN: Low drag configuration enabled"));
	}
	else
	{
		// Restore full downforce
		PhysicsConfig.Aerodynamics.DragCoefficient = 0.38f;
		PhysicsConfig.Aerodynamics.DownforceCoefficient = 1.85f;
		UE_LOG(LogTemp, Display, TEXT("[Supercar] DRS CLOSED: Maximum downforce restored"));
	}
}

void ASupercar::SimulateSupercarHandling(float DeltaTime)
{
	// Calculate chassis slip / drift angle: difference between heading and velocity
	FVector Vel = GetVelocity();
	if (Vel.SizeSquared() > 10000.0f) // > 10 km/h
	{
		FVector Heading = GetActorForwardVector();
		FVector VelNorm = Vel.GetSafeNormal();

		float Dot = FVector::DotProduct(Heading, VelNorm);
		float CrossZ = FVector::CrossProduct(Heading, VelNorm).Z;
		DriftAngleDegrees = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.0f, 1.0f))) * FMath::Sign(CrossZ);
	}
	else
	{
		DriftAngleDegrees = 0.0f;
	}

	// Moderate oversteer tuning:
	// When cornering under power or on sudden throttle lift, induce slight rear yaw rate
	if (FMath::Abs(RawSteering) > 0.2f)
	{
		if (RawThrottle > 0.6f && Telemetry.SpeedKmh > 40.0f)
		{
			// Power-on oversteer: slightly reduce rear tire lateral grip at high slip to let the rear rotate
			Wheels[2].ForceY *= 0.93f;
			Wheels[3].ForceY *= 0.93f;
		}
		else if (RawThrottle < 0.05f && Telemetry.SpeedKmh > 70.0f)
		{
			// Lift-off oversteer characteristic of rear-engine sports cars:
			// Forward weight transfer unloads the rear, allowing crisp turn-in rotation
			Wheels[0].ForceY *= 1.06f;
			Wheels[1].ForceY *= 1.06f;
		}
	}
}
