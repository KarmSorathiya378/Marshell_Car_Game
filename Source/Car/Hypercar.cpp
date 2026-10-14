// Copyright VELOCITY X Racing. All Rights Reserved.

#include "Hypercar.h"

AHypercar::AHypercar()
{
	// Vehicle Identity
	PhysicsConfig.VehicleName = TEXT("Titan Chiron-X (Hypercar)");
	PhysicsConfig.TotalMassKg = 1900.0f; // 1900 kg curb weight with W16 AWD powertrain
	PhysicsConfig.WeightDistributionFront = 0.44f; // 44% front / 56% rear
	PhysicsConfig.WheelbaseCm = 271.1f;
	PhysicsConfig.TrackWidthCm = 175.0f;
	PhysicsConfig.CenterOfMassHeightCm = 36.5f;
	PhysicsConfig.MaxSteeringAngle = 33.0f;
	PhysicsConfig.RatedTopSpeedKmh = 420.0f; // Governed 420 km/h top speed
	PhysicsConfig.Rated0To100TimeSeconds = 2.4f; // 2.4s launch control catapult

	// Engine Parameters: 1500 HP, 1600 N*m 8.0L Quad-Turbo W16
	PhysicsConfig.Engine.MaxHorsepower = 1500.0f;
	PhysicsConfig.Engine.MaxTorque = 1600.0f; // 1600 N*m flat from 2000 to 6000 RPM!
	PhysicsConfig.Engine.IdleRPM = 850.0f;
	PhysicsConfig.Engine.RedlineRPM = 7000.0f;
	PhysicsConfig.Engine.EngineMomentOfInertia = 0.35f; // Quad-turbo heavy crank assembly
	PhysicsConfig.Engine.EngineBrakingRate = 0.20f;
	PhysicsConfig.Engine.RevLimiterCutDuration = 0.08f;
	PhysicsConfig.Engine.PowerToWeightRatio = 789.5f; // 1500 HP / 1.9 tonnes = 789.5 HP/tonne

	// Transmission Parameters: 7-Speed Heavy-Duty Dual-Clutch with Permanent AWD
	PhysicsConfig.Transmission.TransmissionMode = ETransmissionMode::Automatic;
	PhysicsConfig.Transmission.DriveType = EDriveType::AWD;
	// Tall gearing capable of reaching 420 km/h
	PhysicsConfig.Transmission.ForwardGearRatios = { 3.16f, 2.26f, 1.68f, 1.29f, 1.06f, 0.88f, 0.73f };
	PhysicsConfig.Transmission.ReverseGearRatio = 3.10f;
	PhysicsConfig.Transmission.FinalDriveRatio = 3.20f;
	PhysicsConfig.Transmission.GearChangeTime = 0.08f;
	PhysicsConfig.Transmission.AutoUpshiftRPMPercent = 0.94f;
	PhysicsConfig.Transmission.AutoDownshiftRPMPercent = 0.33f;
	PhysicsConfig.Transmission.DifferentialType = EDifferentialType::ActiveTorqueVectoring;
	PhysicsConfig.Transmission.DifferentialLockRatio = 0.80f; // Active electronic locking differentials
	PhysicsConfig.Transmission.FrontTorqueSplit = 0.30f; // Permanent 30% front / 70% rear AWD split

	// Suspension Parameters: Adaptive high-speed pneumatic/hydraulic suspension
	PhysicsConfig.Suspension.SpringRateFront = 95000.0f;  // 95 kN/m
	PhysicsConfig.Suspension.SpringRateRear = 125000.0f;  // 125 kN/m
	PhysicsConfig.Suspension.DampingCompressionFront = 5500.0f;
	PhysicsConfig.Suspension.DampingCompressionRear = 7000.0f;
	PhysicsConfig.Suspension.DampingReboundFront = 8000.0f;
	PhysicsConfig.Suspension.DampingReboundRear = 10500.0f;
	PhysicsConfig.Suspension.RideHeight = 11.5f;          // 11.5 cm default clearance
	PhysicsConfig.Suspension.SuspensionMaxRaise = 7.0f;
	PhysicsConfig.Suspension.SuspensionMaxDrop = 7.0f;
	PhysicsConfig.Suspension.AntiRollBarStiffnessFront = 24000.0f;
	PhysicsConfig.Suspension.AntiRollBarStiffnessRear = 28000.0f;
	PhysicsConfig.Suspension.CamberFront = -1.8f;
	PhysicsConfig.Suspension.CamberRear = -1.5f;

	// Tire Parameters: Bespoke Michelin Pilot Sport Cup 2 (Centrifugal tested to 450 km/h)
	PhysicsConfig.Tires.FrictionMultiplier = 1.72f;
	PhysicsConfig.Tires.CorneringStiffness = 21.0f;
	PhysicsConfig.Tires.LongitudinalStiffness = 28.0f; // Colossal longitudinal traction for 2.4s launch
	PhysicsConfig.Tires.RollingResistance = 0.012f;
	PhysicsConfig.Tires.TireWidthFront = 285.0f;          // 285mm front
	PhysicsConfig.Tires.TireWidthRear = 355.0f;           // Massive 355mm rear steamrollers
	PhysicsConfig.Tires.WheelRadiusFront = 35.5f;
	PhysicsConfig.Tires.WheelRadiusRear = 37.5f;
	PhysicsConfig.Tires.OptimalSlipAngle = 7.0f;
	PhysicsConfig.Tires.OptimalSlipRatio = 0.11f;

	// Aerodynamic Properties: Multi-mode active wing and high-speed diffuser
	PhysicsConfig.Aerodynamics.DragCoefficient = 0.36f;   // Svelte low-drag baseline
	PhysicsConfig.Aerodynamics.FrontalArea = 2.25f;
	PhysicsConfig.Aerodynamics.DownforceCoefficient = 2.15f;
	PhysicsConfig.Aerodynamics.AeroBalanceFront = 0.42f;
	PhysicsConfig.Aerodynamics.AirDensity = 1.225f;
	PhysicsConfig.Aerodynamics.bHasActiveAero = true;
	PhysicsConfig.Aerodynamics.AirbrakeDragMultiplier = 2.6f;      // 0.6G aerodynamic braking
	PhysicsConfig.Aerodynamics.AirbrakeDownforceMultiplier = 1.7f;

	// Driver Assistance: Advanced Electronic Stability Program (ESP) for 1500 HP
	PhysicsConfig.Assistance.bABSEnabled = true;
	PhysicsConfig.Assistance.ABSSlipThreshold = 0.13f;
	PhysicsConfig.Assistance.bTCSEnabled = true;
	PhysicsConfig.Assistance.TCSSlipThreshold = 0.10f; // Keeps 1500 HP hooked to the road
	PhysicsConfig.Assistance.TCSAggressiveness = 0.80f;
	PhysicsConfig.Assistance.bESPEnabled = true;
	PhysicsConfig.Assistance.MaxBrakeForce = 26000.0f;    // 420mm Carbon-Silicon-Carbide brakes
	PhysicsConfig.Assistance.MaxHandbrakeForce = 15000.0f;
	PhysicsConfig.Assistance.BrakeBiasFront = 0.63f;

	CurrentAeroMode = EHypercarAeroMode::Autobahn;
	bTopSpeedKeyActive = false;
	bLaunchControlArmed = false;
	TurboBoostBar = 0.0f;
}

void AHypercar::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("[VELOCITY X] Hypercar initialized with 1500 HP / 1900 kg / 420 km/h AWD powertrain."));
}

void AHypercar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Simulate AWD torque distribution, stability aids, and quad-turbo boost
	SimulateAWDAndStability(DeltaTime);

	// Update active hydraulic rear wing states
	UpdateHypercarAeroWing(DeltaTime);
}

void AHypercar::EngageTopSpeedMode()
{
	bTopSpeedKeyActive = true;
	CurrentAeroMode = EHypercarAeroMode::TopSpeed;

	// Top speed configuration: Drop suspension, flatten wing, reduce drag coefficient to 0.35
	PhysicsConfig.Suspension.RideHeight = 8.0f; // Lowered to 8 cm
	PhysicsConfig.Aerodynamics.DragCoefficient = 0.35f;
	PhysicsConfig.Aerodynamics.DownforceCoefficient = 1.10f; // Minimal drag to breach 400+ km/h
	ActiveAeroAngle = 3.0f; // Almost completely flat wing

	UE_LOG(LogTemp, Display, TEXT("[Hypercar] TOP SPEED KEY ENGAGED: Low drag config active. 420 km/h unlocked!"));
}

void AHypercar::DisengageTopSpeedMode()
{
	bTopSpeedKeyActive = false;
	CurrentAeroMode = EHypercarAeroMode::Handling;

	PhysicsConfig.Suspension.RideHeight = 9.5f;
	PhysicsConfig.Aerodynamics.DragCoefficient = 0.38f;
	PhysicsConfig.Aerodynamics.DownforceCoefficient = 2.15f;
	ActiveAeroAngle = 14.0f;

	UE_LOG(LogTemp, Display, TEXT("[Hypercar] TOP SPEED KEY DISENGAGED: Handling Mode restored"));
}

void AHypercar::ArmLaunchControl()
{
	if (Telemetry.SpeedKmh < 1.0f && RawBrake > 0.8f)
	{
		bLaunchControlArmed = true;
		CurrentGear = 1;
		CurrentRPM = 2800.0f; // Optimal stall RPM for turbo spool
		TurboBoostBar = 1.45f;
		UE_LOG(LogTemp, Display, TEXT("[Hypercar] LAUNCH CONTROL ARMED: 1500 HP Boost Primed. Release brake to launch!"));
	}
}

void AHypercar::SimulateAWDAndStability(float DeltaTime)
{
	// Quad-turbo boost pressure simulation
	if (EffectiveThrottle > 0.1f)
	{
		float TargetBoost = EffectiveThrottle * 1.85f; // Max 1.85 bar boost
		TurboBoostBar = FMath::FInterpTo(TurboBoostBar, TargetBoost, DeltaTime, 5.0f);
	}
	else
	{
		TurboBoostBar = FMath::FInterpTo(TurboBoostBar, 0.0f, DeltaTime, 10.0f);
	}

	// Launch control trigger
	if (bLaunchControlArmed)
	{
		if (RawBrake < 0.2f && RawThrottle > 0.8f)
		{
			// Slingshot launch!
			bLaunchControlArmed = false;
			// AWD ensures zero spinout; channel all 1600 N*m directly into asphalt
			Wheels[0].ForceX *= 1.25f;
			Wheels[1].ForceX *= 1.25f;
			Wheels[2].ForceX *= 1.25f;
			Wheels[3].ForceX *= 1.25f;
			UE_LOG(LogTemp, Display, TEXT("[Hypercar] LAUNCH EXECUTED! 0-100 km/h in 2.4s trajectory"));
		}
	}

	// High-speed yaw stabilization (ESP):
	// At speeds > 250 km/h, hypercar actively counters unwanted yaw divergence
	if (Telemetry.SpeedKmh > 250.0f && FMath::Abs(Telemetry.LateralGForce) > 0.8f)
	{
		// Micro-brake the inside or outside front wheel to prevent spins at high speed
		if (Telemetry.LateralGForce > 0.0f)
		{
			Wheels[1].ForceX -= 1200.0f;
		}
		else
		{
			Wheels[0].ForceX -= 1200.0f;
		}
	}
}

void AHypercar::UpdateHypercarAeroWing(float DeltaTime)
{
	// Airbrake takes top priority under hard high-speed braking
	if (RawBrake > 0.40f && Telemetry.SpeedKmh > 100.0f)
	{
		CurrentAeroMode = EHypercarAeroMode::Airbrake;
		ActiveAeroAngle = FMath::FInterpTo(ActiveAeroAngle, 49.0f, DeltaTime, 14.0f); // 49 degree airbrake
	}
	else if (bTopSpeedKeyActive)
	{
		CurrentAeroMode = EHypercarAeroMode::TopSpeed;
		ActiveAeroAngle = FMath::FInterpTo(ActiveAeroAngle, 3.0f, DeltaTime, 8.0f);
	}
	else if (Telemetry.SpeedKmh > 180.0f)
	{
		CurrentAeroMode = EHypercarAeroMode::Handling;
		ActiveAeroAngle = FMath::FInterpTo(ActiveAeroAngle, 14.0f, DeltaTime, 8.0f);
	}
	else
	{
		CurrentAeroMode = EHypercarAeroMode::Autobahn;
		ActiveAeroAngle = FMath::FInterpTo(ActiveAeroAngle, 0.0f, DeltaTime, 6.0f);
	}
}
