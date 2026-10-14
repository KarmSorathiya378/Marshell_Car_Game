// Copyright VELOCITY X Racing. All Rights Reserved.

#include "TrackSupercar.h"

ATrackSupercar::ATrackSupercar()
{
	// Vehicle Identity
	PhysicsConfig.VehicleName = TEXT("Veloce Senna-R (Track Supercar)");
	PhysicsConfig.TotalMassKg = 1200.0f; // Featherweight carbon construction
	PhysicsConfig.WeightDistributionFront = 0.43f; // 43% front / 57% rear mid-engine balance
	PhysicsConfig.WheelbaseCm = 267.4f;
	PhysicsConfig.TrackWidthCm = 171.0f;
	PhysicsConfig.CenterOfMassHeightCm = 32.0f; // Extremely low center of gravity
	PhysicsConfig.MaxSteeringAngle = 34.0f;
	PhysicsConfig.RatedTopSpeedKmh = 355.0f;
	PhysicsConfig.Rated0To100TimeSeconds = 2.8f;

	// Engine Parameters: 720 HP, 800 N*m Twin-Turbo V8
	PhysicsConfig.Engine.MaxHorsepower = 720.0f;
	PhysicsConfig.Engine.MaxTorque = 800.0f; // N*m @ 5500 - 6700 RPM
	PhysicsConfig.Engine.IdleRPM = 950.0f;
	PhysicsConfig.Engine.RedlineRPM = 8500.0f;
	PhysicsConfig.Engine.EngineMomentOfInertia = 0.18f; // Ultra-light flat-plane crankshaft inertia
	PhysicsConfig.Engine.EngineBrakingRate = 0.26f;
	PhysicsConfig.Engine.RevLimiterCutDuration = 0.05f;
	PhysicsConfig.Engine.PowerToWeightRatio = 600.0f; // 720 HP / 1.2 tonnes = 600 HP/tonne!

	// Transmission Parameters: 7-Speed Seamless Shift Gearbox (SSG)
	PhysicsConfig.Transmission.TransmissionMode = ETransmissionMode::Automatic;
	PhysicsConfig.Transmission.DriveType = EDriveType::RWD;
	PhysicsConfig.Transmission.ForwardGearRatios = { 3.98f, 2.61f, 1.90f, 1.48f, 1.20f, 1.01f, 0.87f };
	PhysicsConfig.Transmission.ReverseGearRatio = 3.60f;
	PhysicsConfig.Transmission.FinalDriveRatio = 3.73f;
	PhysicsConfig.Transmission.GearChangeTime = 0.05f; // Blistering 50ms race shifts
	PhysicsConfig.Transmission.AutoUpshiftRPMPercent = 0.95f;
	PhysicsConfig.Transmission.AutoDownshiftRPMPercent = 0.36f;
	PhysicsConfig.Transmission.DifferentialType = EDifferentialType::LimitedSlip;
	PhysicsConfig.Transmission.DifferentialLockRatio = 0.72f; // Aggressive race ramp LSD
	PhysicsConfig.Transmission.FrontTorqueSplit = 0.0f;

	// Suspension Parameters: Stiff hydraulic RaceActive Chassis Control II
	PhysicsConfig.Suspension.SpringRateFront = 110000.0f; // 110 kN/m ultra-stiff front
	PhysicsConfig.Suspension.SpringRateRear = 135000.0f;  // 135 kN/m rear
	PhysicsConfig.Suspension.DampingCompressionFront = 6200.0f;
	PhysicsConfig.Suspension.DampingCompressionRear = 7800.0f;
	PhysicsConfig.Suspension.DampingReboundFront = 9500.0f;
	PhysicsConfig.Suspension.DampingReboundRear = 11800.0f;
	PhysicsConfig.Suspension.RideHeight = 8.5f;           // Slammed 8.5 cm race ground clearance
	PhysicsConfig.Suspension.SuspensionMaxRaise = 6.0f;
	PhysicsConfig.Suspension.SuspensionMaxDrop = 6.0f;
	PhysicsConfig.Suspension.AntiRollBarStiffnessFront = 28000.0f; // Hydraulically coupled anti-roll
	PhysicsConfig.Suspension.AntiRollBarStiffnessRear = 32000.0f;  // Virtually zero body roll
	PhysicsConfig.Suspension.CamberFront = -3.2f;         // Aggressive negative camber for 2.2G cornering
	PhysicsConfig.Suspension.CamberRear = -2.5f;

	// Tire Parameters: Race compound track slicks (e.g. Pirelli P Zero Trofeo R)
	PhysicsConfig.Tires.FrictionMultiplier = 1.88f;       // Phenomenal track adhesion
	PhysicsConfig.Tires.CorneringStiffness = 24.0f;
	PhysicsConfig.Tires.LongitudinalStiffness = 27.0f;
	PhysicsConfig.Tires.RollingResistance = 0.015f;
	PhysicsConfig.Tires.TireWidthFront = 285.0f;
	PhysicsConfig.Tires.TireWidthRear = 345.0f;
	PhysicsConfig.Tires.WheelRadiusFront = 33.5f;
	PhysicsConfig.Tires.WheelRadiusRear = 35.5f;
	PhysicsConfig.Tires.OptimalSlipAngle = 6.5f;          // Sharp peak cornering slip angle
	PhysicsConfig.Tires.OptimalSlipRatio = 0.10f;

	// Aerodynamic Properties: Monolithic race downforce (McLaren Senna active aero)
	PhysicsConfig.Aerodynamics.DragCoefficient = 0.41f;   // Higher drag due to extreme downforce wing
	PhysicsConfig.Aerodynamics.FrontalArea = 2.05f;
	PhysicsConfig.Aerodynamics.DownforceCoefficient = 2.65f; // Generates > 800 kg downforce at 250 km/h
	PhysicsConfig.Aerodynamics.AeroBalanceFront = 0.44f;  // 44% front downforce keeps front glued in turn-in
	PhysicsConfig.Aerodynamics.AirDensity = 1.225f;
	PhysicsConfig.Aerodynamics.bHasActiveAero = true;
	PhysicsConfig.Aerodynamics.AirbrakeDragMultiplier = 2.8f;      // Huge airbrake deployment drag
	PhysicsConfig.Aerodynamics.AirbrakeDownforceMultiplier = 1.8f; // Sucks rear down under 200+ km/h braking

	// Active Driver Assistance: High-performance race calibration
	PhysicsConfig.Assistance.bABSEnabled = true;
	PhysicsConfig.Assistance.ABSSlipThreshold = 0.12f;
	PhysicsConfig.Assistance.bTCSEnabled = true;
	PhysicsConfig.Assistance.TCSSlipThreshold = 0.09f;
	PhysicsConfig.Assistance.TCSAggressiveness = 0.85f;
	PhysicsConfig.Assistance.bESPEnabled = true;
	PhysicsConfig.Assistance.MaxBrakeForce = 22000.0f;    // Formula-grade carbon ceramic brakes
	PhysicsConfig.Assistance.MaxHandbrakeForce = 13000.0f;
	PhysicsConfig.Assistance.BrakeBiasFront = 0.64f;

	bRaceModeActive = true;
	FrontAeroBladeAngle = 0.0f;
}

void ATrackSupercar::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("[VELOCITY X] Track Supercar initialized with 720 HP / 1200 kg track setup."));
}

void ATrackSupercar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Simulate high-downforce telemetry and active aero surfaces
	SimulateTrackAeroAndGrip(DeltaTime);
}

void ATrackSupercar::ToggleRaceMode()
{
	bRaceModeActive = !bRaceModeActive;

	if (bRaceModeActive)
	{
		// Track/Race Mode: Lower ride height by 35mm, stiffen spring rates
		PhysicsConfig.Suspension.RideHeight = 8.5f;
		PhysicsConfig.Suspension.SpringRateFront = 110000.0f;
		PhysicsConfig.Suspension.SpringRateRear = 135000.0f;
		PhysicsConfig.Aerodynamics.DownforceCoefficient = 2.65f;
		UE_LOG(LogTemp, Display, TEXT("[Track Supercar] RACE MODE ENGAGED: Slammed ride height, max aero downforce"));
	}
	else
	{
		// Road/Transport Mode: Raise ride height for speed bumps, soften damping
		PhysicsConfig.Suspension.RideHeight = 12.0f;
		PhysicsConfig.Suspension.SpringRateFront = 75000.0f;
		PhysicsConfig.Suspension.SpringRateRear = 95000.0f;
		PhysicsConfig.Aerodynamics.DownforceCoefficient = 1.60f;
		UE_LOG(LogTemp, Display, TEXT("[Track Supercar] ROAD MODE ENGAGED: Raised clearance"));
	}
}

void ATrackSupercar::SimulateTrackAeroAndGrip(float DeltaTime)
{
	// Active Front Aero Blades:
	// In corners, actively modulate front aero blade angle to maintain razor-sharp turn-in
	if (FMath::Abs(RawSteering) > 0.1f && Telemetry.SpeedKmh > 80.0f)
	{
		float TargetBladeAngle = FMath::Abs(RawSteering) * 30.0f;
		FrontAeroBladeAngle = FMath::FInterpTo(FrontAeroBladeAngle, TargetBladeAngle, DeltaTime, 16.0f);

		// Dynamic aero loading: high speed downforce increases front grip proportionally to speed squared
		float SpeedRatio = FMath::Clamp(Telemetry.SpeedKmh / 250.0f, 0.0f, 1.4f);
		float AeroGripBonus = 1.0f + (0.12f * SpeedRatio * SpeedRatio);

		Wheels[0].ForceY *= AeroGripBonus;
		Wheels[1].ForceY *= AeroGripBonus;
	}
	else
	{
		FrontAeroBladeAngle = FMath::FInterpTo(FrontAeroBladeAngle, 0.0f, DeltaTime, 10.0f);
	}

	// Active Airbrake Deployment Feedback:
	if (Telemetry.bAirbrakeActive)
	{
		// Massive braking downforce eliminates rear wheel lift and locks the car to the road
		Wheels[2].VerticalLoadN += 3500.0f;
		Wheels[3].VerticalLoadN += 3500.0f;
	}
}
