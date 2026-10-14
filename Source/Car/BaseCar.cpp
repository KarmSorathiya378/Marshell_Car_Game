// Copyright VELOCITY X Racing. All Rights Reserved.

#include "BaseCar.h"
#include "Components/AudioComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
const FName ABaseCar::SocketCockpitCamera = TEXT("Socket_CockpitCamera");
const FName ABaseCar::SocketHoodCamera = TEXT("Socket_HoodCamera");
const FName ABaseCar::SocketChaseBoom = TEXT("Socket_ChaseBoom");
const FName ABaseCar::SocketExhaustLeft = TEXT("Socket_Exhaust_L");
const FName ABaseCar::SocketExhaustRight = TEXT("Socket_Exhaust_R");

ABaseCar::ABaseCar()
{
	PrimaryActorTick.bCanEverTick = true;

	// Default Setup for Wheels (0: FL, 1: FR, 2: RL, 3: RR)
	for (int32 i = 0; i < 4; ++i)
	{
		Wheels[i].WheelIndex = i;
		Wheels[i].AngularVelocity = 0.0f;
		Wheels[i].VisualRotationDegrees = 0.0f;
		Wheels[i].SteerAngleDegrees = 0.0f;
		Wheels[i].SuspensionDisplacementCm = 0.0f;
		Wheels[i].VerticalLoadN = (PhysicsConfig.TotalMassKg * 9.81f) * 0.25f;
		Wheels[i].ContactSurface = ESurfaceType::DryAsphalt;
		Wheels[i].SurfaceFriction = 1.0f;
		Wheels[i].ForceX = 0.0f;
		Wheels[i].ForceY = 0.0f;
		Wheels[i].bIsSkidding = false;
		Wheels[i].bIsLocked = false;
		Wheels[i].bIsSpinning = false;
	}

	// Chase Camera Arm
	ChaseCameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ChaseCameraArm"));
	if (ChaseCameraArm && RootComponent)
	{
		ChaseCameraArm->SetupAttachment(RootComponent);
		ChaseCameraArm->TargetArmLength = 450.0f;
		ChaseCameraArm->SocketOffset = FVector(0.0f, 0.0f, 135.0f);
		ChaseCameraArm->SetRelativeRotation(FRotator(-12.0f, 0.0f, 0.0f));
		ChaseCameraArm->bEnableCameraLag = true;
		ChaseCameraArm->CameraLagSpeed = 12.0f;
		ChaseCameraArm->bEnableCameraRotationLag = true;
		ChaseCameraArm->CameraRotationLagSpeed = 15.0f;
		ChaseCameraArm->bInheritPitch = true;
		ChaseCameraArm->bInheritYaw = true;
		ChaseCameraArm->bInheritRoll = false;
	}

	// Chase Camera
	ChaseCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	if (ChaseCamera && ChaseCameraArm)
	{
		ChaseCamera->SetupAttachment(ChaseCameraArm, USpringArmComponent::SocketName);
		ChaseCamera->FieldOfView = 90.0f;
	}

	// Engine Sound Component
	EngineAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineAudioComponent"));
	if (EngineAudioComponent && RootComponent)
	{
		EngineAudioComponent->SetupAttachment(RootComponent);
		EngineAudioComponent->bAutoActivate = false;
	}

	// Headlights (Left & Right)
	HeadlightLeft = CreateDefaultSubobject<USpotLightComponent>(TEXT("HeadlightLeft"));
	if (HeadlightLeft && RootComponent)
	{
		HeadlightLeft->SetupAttachment(RootComponent);
		HeadlightLeft->SetRelativeLocation(FVector(195.0f, -65.0f, 35.0f));
		HeadlightLeft->SetRelativeRotation(FRotator(-2.0f, 0.0f, 0.0f));
		HeadlightLeft->Intensity = 50000.0f;
		HeadlightLeft->AttenuationRadius = 4500.0f;
		HeadlightLeft->InnerConeAngle = 18.0f;
		HeadlightLeft->OuterConeAngle = 35.0f;
		HeadlightLeft->SetVisibility(false);
	}

	HeadlightRight = CreateDefaultSubobject<USpotLightComponent>(TEXT("HeadlightRight"));
	if (HeadlightRight && RootComponent)
	{
		HeadlightRight->SetupAttachment(RootComponent);
		HeadlightRight->SetRelativeLocation(FVector(195.0f, 65.0f, 35.0f));
		HeadlightRight->SetRelativeRotation(FRotator(-2.0f, 0.0f, 0.0f));
		HeadlightRight->Intensity = 50000.0f;
		HeadlightRight->AttenuationRadius = 4500.0f;
		HeadlightRight->InnerConeAngle = 18.0f;
		HeadlightRight->OuterConeAngle = 35.0f;
		HeadlightRight->SetVisibility(false);
	}

	// Taillights (Left & Right)
	TaillightLeft = CreateDefaultSubobject<UPointLightComponent>(TEXT("TaillightLeft"));
	if (TaillightLeft && RootComponent)
	{
		TaillightLeft->SetupAttachment(RootComponent);
		TaillightLeft->SetRelativeLocation(FVector(-195.0f, -65.0f, 40.0f));
		TaillightLeft->SetLightColor(FLinearColor(1.0f, 0.02f, 0.02f));
		TaillightLeft->Intensity = 2500.0f;
		TaillightLeft->AttenuationRadius = 600.0f;
	}

	TaillightRight = CreateDefaultSubobject<UPointLightComponent>(TEXT("TaillightRight"));
	if (TaillightRight && RootComponent)
	{
		TaillightRight->SetupAttachment(RootComponent);
		TaillightRight->SetRelativeLocation(FVector(-195.0f, 65.0f, 40.0f));
		TaillightRight->SetLightColor(FLinearColor(1.0f, 0.02f, 0.02f));
		TaillightRight->Intensity = 2500.0f;
		TaillightRight->AttenuationRadius = 600.0f;
	}

	// Reverse Backup Light
	ReverseLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ReverseLight"));
	if (ReverseLight && RootComponent)
	{
		ReverseLight->SetupAttachment(RootComponent);
		ReverseLight->SetRelativeLocation(FVector(-198.0f, 0.0f, 30.0f));
		ReverseLight->SetLightColor(FLinearColor(1.0f, 1.0f, 0.95f));
		ReverseLight->Intensity = 3000.0f;
		ReverseLight->AttenuationRadius = 700.0f;
		ReverseLight->SetVisibility(false);
	}

	bHeadlightsOn = false;
	CurrentRPM = PhysicsConfig.Engine.IdleRPM;
	CurrentGear = 1;
}

void ABaseCar::BeginPlay()
{
	Super::BeginPlay();

	// Initialize dynamic variables
	CurrentRPM = PhysicsConfig.Engine.IdleRPM;
	CurrentGear = 1;
	PreviousVelocity = GetVelocity();

	// Activate engine audio if assigned
	if (EngineAudioComponent)
	{
		EngineAudioComponent->Play();
	}

	UE_LOG(LogTemp, Display, TEXT("[VELOCITY X] %s initialized. Mass: %.1f kg, Power: %.0f HP, TopSpeed: %.0f km/h"),
		*PhysicsConfig.VehicleName, PhysicsConfig.TotalMassKg, PhysicsConfig.Engine.MaxHorsepower, PhysicsConfig.RatedTopSpeedKmh);
}

void ABaseCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DeltaTime <= 0.0f)
	{
		return;
	}

	// 1. Calculate Acceleration and G-Forces from Velocity Change
	FVector CurrentVel = GetVelocity();
	FVector ForwardVec = GetActorForwardVector();
	FVector RightVec = GetActorRightVector();

	CurrentAcceleration = (CurrentVel - PreviousVelocity) / DeltaTime;
	PreviousVelocity = CurrentVel;

	float ForwardSpeedCmS = FVector::DotProduct(CurrentVel, ForwardVec);
	float SpeedMps = ForwardSpeedCmS * 0.01f;
	float SpeedKmh = SpeedMps * 3.6f;

	float LongAccelMps2 = FVector::DotProduct(CurrentAcceleration, ForwardVec) * 0.01f;
	float LatAccelMps2 = FVector::DotProduct(CurrentAcceleration, RightVec) * 0.01f;

	Telemetry.LongitudinalGForce = LongAccelMps2 / 9.81f;
	Telemetry.LateralGForce = LatAccelMps2 / 9.81f;
	Telemetry.SpeedKmh = FMath::Abs(SpeedKmh);
	Telemetry.SpeedMph = Telemetry.SpeedKmh * 0.621371f;

	// 2. ABS and Traction Control Modulation
	ApplyABSAndTractionControl(DeltaTime);

	// 3. Weight Transfer Simulation (Pitch & Roll)
	SimulateWeightTransfer(DeltaTime, LongAccelMps2, LatAccelMps2);

	// 4. Powertrain, Engine RPM & Transmission Simulation
	SimulateEngineAndDrivetrain(DeltaTime);

	// 5. Differential Locking Simulation
	float EngineTorqueNm = CalculateTorqueAtRPM(CurrentRPM) * EffectiveThrottle;
	SimulateDifferential(DeltaTime, EngineTorqueNm);

	// 6. Engine Braking Simulation
	SimulateEngineBraking(DeltaTime);

	// 7. Wheel Slip and Surface Traction Calculations
	CalculateWheelSlip(DeltaTime);

	for (int32 i = 0; i < 4; ++i)
	{
		Wheels[i].ContactSurface = DetectSurfaceUnderWheel(i);
		Wheels[i].SurfaceFriction = GetSurfaceFrictionCoefficient(Wheels[i].ContactSurface);
		CalculateSurfaceTraction(i, Wheels[i].ContactSurface);
	}

	// 8. Aerodynamic Drag and Downforce Simulation
	SimulateAerodynamics(DeltaTime);

	// 9. Visual Physics Updates
	UpdateVisualWheelRotation(DeltaTime);
	UpdateVisualSuspension(DeltaTime);
	UpdateVisualLights(DeltaTime);
	UpdateAudioPitchAndVolume(DeltaTime);

	// 10. Update Telemetry
	Telemetry.EngineRPM = CurrentRPM;
	Telemetry.CurrentGear = CurrentGear;
	Telemetry.ThrottleInput = RawThrottle;
	Telemetry.BrakeInput = RawBrake;
	Telemetry.SteeringInput = RawSteering;
}

void ABaseCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseCar::SetPhysicsSettings(const FVehiclePhysicsConfig& NewConfig)
{
	PhysicsConfig = NewConfig;
	UE_LOG(LogTemp, Display, TEXT("[VELOCITY X] Updated physics settings for %s"), *PhysicsConfig.VehicleName);
}

FWheelPhysicsState ABaseCar::GetWheelState(int32 WheelIndex) const
{
	if (WheelIndex >= 0 && WheelIndex < 4)
	{
		return Wheels[WheelIndex];
	}
	return FWheelPhysicsState();
}

// ============================================================================
// DRIVER INPUTS
// ============================================================================

void ABaseCar::ApplyThrottle(float Value)
{
	RawThrottle = FMath::Clamp(Value, 0.0f, 1.0f);
}

void ABaseCar::ApplyBrake(float Value)
{
	RawBrake = FMath::Clamp(Value, 0.0f, 1.0f);
}

void ABaseCar::ApplySteering(float Value)
{
	RawSteering = FMath::Clamp(Value, -1.0f, 1.0f);
}

void ABaseCar::ApplyHandbrake(bool bActive)
{
	bRawHandbrake = bActive;
}

void ABaseCar::ShiftUp()
{
	if (bIsShifting)
	{
		return;
	}

	int32 MaxGear = PhysicsConfig.Transmission.ForwardGearRatios.Num();
	if (CurrentGear < MaxGear)
	{
		CurrentGear++;
		bIsShifting = true;
		ShiftTimer = PhysicsConfig.Transmission.GearChangeTime;

		// Drop RPM slightly on upshift
		CurrentRPM = FMath::Max(PhysicsConfig.Engine.IdleRPM, CurrentRPM * 0.72f);
		UE_LOG(LogTemp, Log, TEXT("[Transmission] Upshift to gear %d"), CurrentGear);
		OnGearChange.Broadcast(CurrentGear);
	}
}

void ABaseCar::ShiftDown()
{
	if (bIsShifting)
	{
		return;
	}

	if (CurrentGear > -1)
	{
		CurrentGear--;
		bIsShifting = true;
		ShiftTimer = PhysicsConfig.Transmission.GearChangeTime;

		// Blip throttle / raise RPM on downshift
		CurrentRPM = FMath::Min(PhysicsConfig.Engine.RedlineRPM, CurrentRPM * 1.35f);
		UE_LOG(LogTemp, Log, TEXT("[Transmission] Downshift to gear %d"), CurrentGear);
		OnGearChange.Broadcast(CurrentGear);
	}
}

void ABaseCar::ToggleTransmissionMode()
{
	if (PhysicsConfig.Transmission.TransmissionMode == ETransmissionMode::Automatic)
	{
		PhysicsConfig.Transmission.TransmissionMode = ETransmissionMode::Manual;
		UE_LOG(LogTemp, Display, TEXT("[Transmission] Switched to MANUAL mode"));
	}
	else
	{
		PhysicsConfig.Transmission.TransmissionMode = ETransmissionMode::Automatic;
		UE_LOG(LogTemp, Display, TEXT("[Transmission] Switched to AUTOMATIC mode"));
	}
}

void ABaseCar::SetAutomaticTransmission(bool bEnable)
{
	PhysicsConfig.Transmission.TransmissionMode = bEnable ? ETransmissionMode::Automatic : ETransmissionMode::Manual;
	UE_LOG(LogTemp, Display, TEXT("[Transmission] Mode set to %s"), bEnable ? TEXT("AUTOMATIC") : TEXT("MANUAL"));
}

void ABaseCar::ToggleHeadlights()
{
	bHeadlightsOn = !bHeadlightsOn;
	if (HeadlightLeft)
	{
		HeadlightLeft->SetVisibility(bHeadlightsOn);
	}
	if (HeadlightRight)
	{
		HeadlightRight->SetVisibility(bHeadlightsOn);
	}
	UE_LOG(LogTemp, Log, TEXT("[VELOCITY X] Headlights: %s"), bHeadlightsOn ? TEXT("ON") : TEXT("OFF"));
}

void ABaseCar::ResetVehicle()
{
	ResetVehicleAtTransform(FTransform(FRotator(0.0f, GetActorRotation().Yaw, 0.0f), GetActorLocation() + FVector(0.0f, 0.0f, 40.0f)));
}

void ABaseCar::ResetVehicleAtTransform(const FTransform& TargetTransform)
{
	SetActorTransform(TargetTransform, false, nullptr, ETeleportType::TeleportPhysics);

	// Reset velocities
	UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(GetRootComponent());
	if (PrimComp && PrimComp->IsSimulatingPhysics())
	{
		PrimComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
		PrimComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}

	CurrentRPM = PhysicsConfig.Engine.IdleRPM;
	CurrentGear = 1;
	PreviousVelocity = FVector::ZeroVector;

	for (int32 i = 0; i < 4; ++i)
	{
		Wheels[i].AngularVelocity = 0.0f;
		Wheels[i].SlipRatio = 0.0f;
		Wheels[i].SlipAngleDegrees = 0.0f;
		Wheels[i].bIsSkidding = false;
		Wheels[i].bIsLocked = false;
		Wheels[i].bIsSpinning = false;
	}

	UE_LOG(LogTemp, Display, TEXT("[VELOCITY X] Vehicle reset to transform"));
}

FString ABaseCar::GetGearName() const
{
	if (CurrentGear == -1)
	{
		return TEXT("R");
	}
	if (CurrentGear == 0)
	{
		return TEXT("N");
	}
	return FString::FromInt(CurrentGear);
}

float ABaseCar::GetWheelSlipRatio(int32 WheelIndex) const
{
	if (WheelIndex >= 0 && WheelIndex < 4)
	{
		return Wheels[WheelIndex].SlipRatio;
	}
	return 0.0f;
}

float ABaseCar::GetSuspensionCompression(int32 WheelIndex) const
{
	if (WheelIndex >= 0 && WheelIndex < 4)
	{
		return Wheels[WheelIndex].SuspensionDisplacementCm;
	}
	return 0.0f;
}

// ============================================================================
// SURFACE FRICTION COEFFICIENTS
// ============================================================================

float ABaseCar::GetSurfaceFrictionCoefficient(ESurfaceType Surface)
{
	switch (Surface)
	{
	case ESurfaceType::DryAsphalt:
		return 1.0f;
	case ESurfaceType::WetAsphalt:
		return 0.65f;
	case ESurfaceType::TrackRubber:
		return 1.18f;
	case ESurfaceType::Dirt:
		return 0.40f;
	case ESurfaceType::Grass:
		return 0.28f;
	case ESurfaceType::Ice:
		return 0.12f;
	case ESurfaceType::Curb:
		return 0.82f;
	default:
		return 1.0f;
	}
}

ESurfaceType ABaseCar::DetectSurfaceUnderWheel(int32 WheelIndex) const
{
	if (!GetWorld())
	{
		return ESurfaceType::DryAsphalt;
	}

	FVector TraceStart = GetActorLocation();
	// Offset traces to wheel locations
	float HalfWheelbase = (PhysicsConfig.WheelbaseCm * 0.5f);
	float HalfTrack = (PhysicsConfig.TrackWidthCm * 0.5f);

	float XOffset = (WheelIndex < 2) ? HalfWheelbase : -HalfWheelbase;
	float YOffset = (WheelIndex % 2 == 0) ? -HalfTrack : HalfTrack;

	FVector WheelLocation = GetActorLocation() + GetActorForwardVector() * XOffset + GetActorRightVector() * YOffset;
	FVector TraceEnd = WheelLocation - FVector(0.0f, 0.0f, 80.0f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bReturnPhysicalMaterial = true;

	if (GetWorld()->LineTraceSingleByChannel(Hit, WheelLocation + FVector(0, 0, 30.0f), TraceEnd, ECC_Visibility, Params))
	{
		// Physical material check or tag evaluation
		if (Hit.Component.IsValid())
		{
			FString CompName = Hit.Component->GetName().ToLower();
			if (CompName.Contains(TEXT("wet")))
			{
				return ESurfaceType::WetAsphalt;
			}
			if (CompName.Contains(TEXT("dirt")) || CompName.Contains(TEXT("gravel")))
			{
				return ESurfaceType::Dirt;
			}
			if (CompName.Contains(TEXT("grass")))
			{
				return ESurfaceType::Grass;
			}
			if (CompName.Contains(TEXT("ice")) || CompName.Contains(TEXT("snow")))
			{
				return ESurfaceType::Ice;
			}
			if (CompName.Contains(TEXT("curb")) || CompName.Contains(TEXT("kerb")))
			{
				return ESurfaceType::Curb;
			}
		}
	}

	return ESurfaceType::DryAsphalt;
}

// ============================================================================
// WEIGHT TRANSFER (PITCH & ROLL)
// ============================================================================

void ABaseCar::SimulateWeightTransfer(float DeltaTime, float LongitudinalAccel, float LateralAccel)
{
	float Mass = PhysicsConfig.TotalMassKg;
	float Gravity = 9.81f;
	float TotalWeight = Mass * Gravity;

	// Baseline static weight distribution
	float FrontStaticRatio = PhysicsConfig.WeightDistributionFront;
	float RearStaticRatio = 1.0f - FrontStaticRatio;

	float FrontStaticWeight = TotalWeight * FrontStaticRatio;
	float RearStaticWeight = TotalWeight * RearStaticRatio;

	// Longitudinal Weight Transfer: DeltaFz = (m * a_x * h_cg) / L
	float WheelbaseMeters = FMath::Max(1.0f, PhysicsConfig.WheelbaseCm * 0.01f);
	float ComHeightMeters = PhysicsConfig.CenterOfMassHeightCm * 0.01f;
	float DeltaFzPitch = (Mass * LongitudinalAccel * ComHeightMeters) / WheelbaseMeters;

	// Lateral Weight Transfer: DeltaFz = (m * a_y * h_cg) / TrackWidth
	float TrackWidthMeters = FMath::Max(1.0f, PhysicsConfig.TrackWidthCm * 0.01f);
	float DeltaFzRoll = (Mass * LateralAccel * ComHeightMeters) / TrackWidthMeters;

	// Distribute loads across 4 wheels
	// Pitch: Under braking (LongitudinalAccel < 0), DeltaFzPitch is negative, shifts weight to front
	// Roll: Left turn (LateralAccel > 0), load transfers to right wheels
	float FrontAxleLoad = FrontStaticWeight - DeltaFzPitch;
	float RearAxleLoad = RearStaticWeight + DeltaFzPitch;

	// Clamp to prevent wheels having negative normal force (wheel lift)
	FrontAxleLoad = FMath::Max(200.0f, FrontAxleLoad);
	RearAxleLoad = FMath::Max(200.0f, RearAxleLoad);

	// Distribute lateral load according to anti-roll bar stiffness bias
	float FrontArbRatio = PhysicsConfig.Suspension.AntiRollBarStiffnessFront /
		(PhysicsConfig.Suspension.AntiRollBarStiffnessFront + PhysicsConfig.Suspension.AntiRollBarStiffnessRear);
	float RearArbRatio = 1.0f - FrontArbRatio;

	float FrontDeltaRoll = DeltaFzRoll * FrontArbRatio;
	float RearDeltaRoll = DeltaFzRoll * RearArbRatio;

	// 0: Front-Left (inside on right turn, outside on left turn)
	Wheels[0].VerticalLoadN = FMath::Max(50.0f, (FrontAxleLoad * 0.5f) - FrontDeltaRoll);
	// 1: Front-Right
	Wheels[1].VerticalLoadN = FMath::Max(50.0f, (FrontAxleLoad * 0.5f) + FrontDeltaRoll);
	// 2: Rear-Left
	Wheels[2].VerticalLoadN = FMath::Max(50.0f, (RearAxleLoad * 0.5f) - RearDeltaRoll);
	// 3: Rear-Right
	Wheels[3].VerticalLoadN = FMath::Max(50.0f, (RearAxleLoad * 0.5f) + RearDeltaRoll);

	// Chassis visual dive and roll angles
	float TargetPitch = -(LongitudinalAccel / Gravity) * 3.8f; // Dive under braking, squat under accel
	float TargetRoll = (LateralAccel / Gravity) * 4.2f;        // Roll in corners

	ChassisPitch = FMath::FInterpTo(ChassisPitch, TargetPitch, DeltaTime, 12.0f);
	ChassisRoll = FMath::FInterpTo(ChassisRoll, TargetRoll, DeltaTime, 14.0f);

	Telemetry.ChassisPitchDegrees = ChassisPitch;
	Telemetry.ChassisRollDegrees = ChassisRoll;
}

// ============================================================================
// WHEEL SLIP & TRACTION CALCULATIONS (PACEJKA MAGIC FORMULA)
// ============================================================================

void ABaseCar::CalculateWheelSlip(float DeltaTime)
{
	FVector ForwardVec = GetActorForwardVector();
	FVector RightVec = GetActorRightVector();
	FVector Vel = GetVelocity();

	float ChassisSpeedX = FVector::DotProduct(Vel, ForwardVec) * 0.01f; // m/s
	float ChassisSpeedY = FVector::DotProduct(Vel, RightVec) * 0.01f;   // m/s

	for (int32 i = 0; i < 4; ++i)
	{
		float WheelRadiusM = ((i < 2) ? PhysicsConfig.Tires.WheelRadiusFront : PhysicsConfig.Tires.WheelRadiusRear) * 0.01f;
		float WheelLinearSpeed = Wheels[i].AngularVelocity * WheelRadiusM;

		// Longitudinal Slip Ratio s_x = (omega * R - Vx) / max(|Vx|, |omega * R|, epsilon)
		float MaxVel = FMath::Max3(FMath::Abs(ChassisSpeedX), FMath::Abs(WheelLinearSpeed), 0.1f);
		Wheels[i].SlipRatio = (WheelLinearSpeed - ChassisSpeedX) / MaxVel;
		Wheels[i].SlipRatio = FMath::Clamp(Wheels[i].SlipRatio, -1.0f, 1.0f);

		// Steer angle for front wheels
		if (i < 2)
		{
			// Speed-sensitive steering reduction to prevent twitchiness at 300+ km/h
			float SpeedKmh = FMath::Abs(ChassisSpeedX * 3.6f);
			float SpeedSteerFactor = FMath::GetMappedRangeValueClamped(FVector2D(50.0f, 320.0f), FVector2D(1.0f, 0.28f), SpeedKmh);
			Wheels[i].SteerAngleDegrees = RawSteering * PhysicsConfig.MaxSteeringAngle * SpeedSteerFactor;
		}
		else
		{
			Wheels[i].SteerAngleDegrees = 0.0f;
		}

		// Lateral Slip Angle alpha = -atan2(Vy, |Vx| + epsilon) in degrees
		float EffectiveVy = ChassisSpeedY;
		if (i < 2)
		{
			// Take steering angle into account for front wheels
			float SteerRad = FMath::DegreesToRadians(Wheels[i].SteerAngleDegrees);
			EffectiveVy -= ChassisSpeedX * FMath::Tan(SteerRad);
		}

		float SlipAngleRad = -FMath::Atan2(EffectiveVy, FMath::Abs(ChassisSpeedX) + 0.3f);
		Wheels[i].SlipAngleDegrees = FMath::RadiansToDegrees(SlipAngleRad);

		// Status flags
		Wheels[i].bIsLocked = (RawBrake > 0.2f && FMath::Abs(WheelLinearSpeed) < 0.5f && FMath::Abs(ChassisSpeedX) > 3.0f);
		Wheels[i].bIsSpinning = (RawThrottle > 0.3f && Wheels[i].SlipRatio > PhysicsConfig.Assistance.TCSSlipThreshold);
		Wheels[i].bIsSkidding = (FMath::Abs(Wheels[i].SlipAngleDegrees) > PhysicsConfig.Tires.OptimalSlipAngle * 1.5f ||
			FMath::Abs(Wheels[i].SlipRatio) > PhysicsConfig.Tires.OptimalSlipRatio * 1.8f);

		if (Wheels[i].bIsSkidding || Wheels[i].bIsSpinning)
		{
			OnTireSlip.Broadcast(i, Wheels[i].SlipRatio);
		}
	}
}

void ABaseCar::CalculateSurfaceTraction(int32 WheelIndex, ESurfaceType Surface)
{
	FWheelPhysicsState& Wheel = Wheels[WheelIndex];
	float MuSurface = Wheel.SurfaceFriction * PhysicsConfig.Tires.FrictionMultiplier;
	float Fz = Wheel.VerticalLoadN;

	// Pacejka Magic Formula Approximation:
	// F = D * sin(C * atan(B * x - E * (B * x - atan(B * x))))
	// Longitudinal traction:
	float Bx = PhysicsConfig.Tires.LongitudinalStiffness;
	float Cx = 1.65f;
	float Dx = MuSurface * Fz;
	float Sx = Wheel.SlipRatio;

	float LongForce = Dx * FMath::Sin(Cx * FMath::Atan(Bx * Sx));

	// Lateral cornering force:
	float By = PhysicsConfig.Tires.CorneringStiffness;
	float Cy = 1.35f;
	float Dy = MuSurface * Fz;
	float Alpha = FMath::DegreesToRadians(Wheel.SlipAngleDegrees);

	float LatForce = Dy * FMath::Sin(Cy * FMath::Atan(By * Alpha));

	// Combined Slip Friction Ellipse Clamping: (Fx / Fx_max)^2 + (Fy / Fy_max)^2 <= 1
	float MaxTraction = MuSurface * Fz;
	float CombinedVectorLength = FMath::Sqrt(LongForce * LongForce + LatForce * LatForce);

	if (CombinedVectorLength > MaxTraction && CombinedVectorLength > 0.001f)
	{
		float Scale = MaxTraction / CombinedVectorLength;
		LongForce *= Scale;
		LatForce *= Scale;
	}

	Wheel.ForceX = LongForce;
	Wheel.ForceY = LatForce;
}

// ============================================================================
// POWERTRAIN, ENGINE & GEAR RATIOS
// ============================================================================

float ABaseCar::CalculateTorqueAtRPM(float RPM) const
{
	float Idle = PhysicsConfig.Engine.IdleRPM;
	float Redline = PhysicsConfig.Engine.RedlineRPM;
	float PeakTorque = PhysicsConfig.Engine.MaxTorque;

	if (RPM < Idle)
	{
		return PeakTorque * 0.4f;
	}

	// Normalized RPM between Idle and Redline
	float NormalizedRPM = (RPM - Idle) / (Redline - Idle);

	// Realistic high-performance torque curve:
	// Peak torque typically plateaus around 60-75% of redline RPM
	float TorqueFactor;
	if (NormalizedRPM < 0.25f)
	{
		// Low RPM ramp-up
		TorqueFactor = FMath::Lerp(0.55f, 0.90f, NormalizedRPM / 0.25f);
	}
	else if (NormalizedRPM < 0.70f)
	{
		// Torque plateau
		TorqueFactor = FMath::Lerp(0.90f, 1.0f, (NormalizedRPM - 0.25f) / 0.45f);
	}
	else
	{
		// High RPM slight taper near redline while horsepower peaks
		TorqueFactor = FMath::Lerp(1.0f, 0.88f, (NormalizedRPM - 0.70f) / 0.30f);
	}

	return PeakTorque * TorqueFactor;
}

void ABaseCar::SimulateEngineAndDrivetrain(float DeltaTime)
{
	// Manage Shift Delays
	if (bIsShifting)
	{
		ShiftTimer -= DeltaTime;
		if (ShiftTimer <= 0.0f)
		{
			bIsShifting = false;
		}
	}

	// Manage Rev Limiter Bouncing
	if (bRevLimiterCutting)
	{
		RevLimiterTimer -= DeltaTime;
		if (RevLimiterTimer <= 0.0f)
		{
			bRevLimiterCutting = false;
		}
	}

	// Automatic Shifting Logic
	if (PhysicsConfig.Transmission.TransmissionMode == ETransmissionMode::Automatic && !bIsShifting)
	{
		int32 MaxGear = PhysicsConfig.Transmission.ForwardGearRatios.Num();
		float UpshiftRPM = PhysicsConfig.Engine.RedlineRPM * PhysicsConfig.Transmission.AutoUpshiftRPMPercent;
		float DownshiftRPM = PhysicsConfig.Engine.RedlineRPM * PhysicsConfig.Transmission.AutoDownshiftRPMPercent;

		if (CurrentGear > 0 && CurrentGear < MaxGear && CurrentRPM >= UpshiftRPM && RawThrottle > 0.4f)
		{
			ShiftUp();
		}
		else if (CurrentGear > 1 && CurrentRPM <= DownshiftRPM && RawBrake < 0.1f)
		{
			ShiftDown();
		}
		else if (CurrentGear == 1 && RawBrake > 0.5f && Telemetry.SpeedKmh < 2.0f)
		{
			// Automatic switch to reverse when stationary and holding brake
			CurrentGear = -1;
		}
		else if (CurrentGear == -1 && RawThrottle > 0.5f && Telemetry.SpeedKmh < 2.0f)
		{
			// Automatic switch back to 1st gear
			CurrentGear = 1;
		}
	}

	// Compute Engine RPM from road speed or free revving
	float CurrentSpeedMps = Telemetry.SpeedKmh / 3.6f;
	float GearRatio = 1.0f;

	if (CurrentGear > 0 && CurrentGear <= PhysicsConfig.Transmission.ForwardGearRatios.Num())
	{
		GearRatio = PhysicsConfig.Transmission.ForwardGearRatios[CurrentGear - 1];
	}
	else if (CurrentGear == -1)
	{
		GearRatio = PhysicsConfig.Transmission.ReverseGearRatio;
	}

	float TotalDriveRatio = GearRatio * PhysicsConfig.Transmission.FinalDriveRatio;
	float RearWheelRadiusM = PhysicsConfig.Tires.WheelRadiusRear * 0.01f;

	// Mechanical RPM tied to wheel speed when in gear
	float WheelRPM = (CurrentSpeedMps / (2.0f * PI * RearWheelRadiusM)) * 60.0f;
	float MechanicalEngineRPM = WheelRPM * TotalDriveRatio;

	if (bIsShifting || CurrentGear == 0)
	{
		// Free revving when clutch is disengaged or neutral
		float RevTarget = (EffectiveThrottle > 0.05f) ? PhysicsConfig.Engine.RedlineRPM * EffectiveThrottle : PhysicsConfig.Engine.IdleRPM;
		float InertiaRate = 1.0f / FMath::Max(0.05f, PhysicsConfig.Engine.EngineMomentOfInertia);
		CurrentRPM = FMath::FInterpTo(CurrentRPM, RevTarget, DeltaTime, 14.0f * InertiaRate);
	}
	else
	{
		// Interp towards mechanical RPM with throttle contribution
		float TargetRPM = FMath::Max(MechanicalEngineRPM, PhysicsConfig.Engine.IdleRPM);
		if (EffectiveThrottle > 0.05f && MechanicalEngineRPM < PhysicsConfig.Engine.IdleRPM * 1.5f)
		{
			// Slip clutch during low speed launch
			TargetRPM = FMath::Max(TargetRPM, PhysicsConfig.Engine.IdleRPM + (EffectiveThrottle * 2500.0f));
		}

		CurrentRPM = FMath::FInterpTo(CurrentRPM, TargetRPM, DeltaTime, 18.0f);
	}

	// Rev Limiter Protection
	if (CurrentRPM >= PhysicsConfig.Engine.RedlineRPM)
	{
		CurrentRPM = PhysicsConfig.Engine.RedlineRPM;
		bRevLimiterCutting = true;
		RevLimiterTimer = PhysicsConfig.Engine.RevLimiterCutDuration;
	}

	CurrentRPM = FMath::Clamp(CurrentRPM, PhysicsConfig.Engine.IdleRPM, PhysicsConfig.Engine.RedlineRPM);
}

// ============================================================================
// DIFFERENTIAL LOCKING
// ============================================================================

void ABaseCar::SimulateDifferential(float DeltaTime, float EngineDriveTorque)
{
	float RearWheelRadiusM = PhysicsConfig.Tires.WheelRadiusRear * 0.01f;

	// Driven wheels: Rear wheels 2 (Left) and 3 (Right) for RWD
	float OmegaL = Wheels[2].AngularVelocity;
	float OmegaR = Wheels[3].AngularVelocity;
	float DeltaOmega = OmegaL - OmegaR; // Angular speed discrepancy

	float TorqueL = EngineDriveTorque * 0.5f;
	float TorqueR = EngineDriveTorque * 0.5f;

	switch (PhysicsConfig.Transmission.DifferentialType)
	{
	case EDifferentialType::Open:
		// Open diff maintains equal torque across both sides, so spinning wheel robs power
		break;

	case EDifferentialType::LimitedSlip:
	{
		// Mechanical LSD: lock torque transfers power away from spinning wheel
		float LockRatio = PhysicsConfig.Transmission.DifferentialLockRatio;
		float TransferTorque = LockRatio * FMath::Abs(EngineDriveTorque) * FMath::Sign(DeltaOmega);

		TorqueL -= TransferTorque * 0.5f;
		TorqueR += TransferTorque * 0.5f;
		break;
	}

	case EDifferentialType::Locked:
	{
		// Spool locks wheels to identical rotational rate
		float AvgOmega = (OmegaL + OmegaR) * 0.5f;
		Wheels[2].AngularVelocity = AvgOmega;
		Wheels[3].AngularVelocity = AvgOmega;
		break;
	}

	case EDifferentialType::ActiveTorqueVectoring:
	{
		// Active differential: vector torque to outside wheel to generate yaw moment into corner
		float YawVectorBias = RawSteering * 0.25f; // Bias up to 25% extra torque to outer wheel
		TorqueL = EngineDriveTorque * (0.5f - YawVectorBias);
		TorqueR = EngineDriveTorque * (0.5f + YawVectorBias);
		break;
	}
	}

	// AWD Torque Transfer to front axle if configured
	if (PhysicsConfig.Transmission.DriveType == EDriveType::AWD && PhysicsConfig.Transmission.FrontTorqueSplit > 0.0f)
	{
		float FrontSplit = PhysicsConfig.Transmission.FrontTorqueSplit;
		float FrontTorque = EngineDriveTorque * FrontSplit * 0.5f;
		float FrontWheelRadiusM = PhysicsConfig.Tires.WheelRadiusFront * 0.01f;

		Wheels[0].AngularVelocity += (FrontTorque / (PhysicsConfig.TotalMassKg * 0.25f * FrontWheelRadiusM * FrontWheelRadiusM)) * DeltaTime;
		Wheels[1].AngularVelocity += (FrontTorque / (PhysicsConfig.TotalMassKg * 0.25f * FrontWheelRadiusM * FrontWheelRadiusM)) * DeltaTime;

		TorqueL *= (1.0f - FrontSplit);
		TorqueR *= (1.0f - FrontSplit);
	}

	// Integrate driven wheel angular velocities
	float WheelInertia = 1.8f; // kg*m^2 per wheel assembly
	Wheels[2].AngularVelocity += (TorqueL / WheelInertia) * DeltaTime;
	Wheels[3].AngularVelocity += (TorqueR / WheelInertia) * DeltaTime;
}

// ============================================================================
// ENGINE BRAKING
// ============================================================================

void ABaseCar::SimulateEngineBraking(float DeltaTime)
{
	// Active when driver is off throttle and vehicle is moving in gear
	if (RawThrottle < 0.05f && CurrentGear != 0 && Telemetry.SpeedKmh > 5.0f && !bIsShifting)
	{
		float NormalizedRPM = (CurrentRPM - PhysicsConfig.Engine.IdleRPM) /
			(PhysicsConfig.Engine.RedlineRPM - PhysicsConfig.Engine.IdleRPM);
		NormalizedRPM = FMath::Clamp(NormalizedRPM, 0.0f, 1.0f);

		// Engine braking torque proportional to RPM and engine braking constant
		float EngineBrakingTorque = PhysicsConfig.Engine.MaxTorque * PhysicsConfig.Engine.EngineBrakingRate * (NormalizedRPM + 0.15f);

		// Retard driven wheel angular velocity
		float WheelInertia = 1.8f;
		float BrakeDecel = (EngineBrakingTorque / WheelInertia) * DeltaTime;

		Wheels[2].AngularVelocity = FMath::FInterpTo(Wheels[2].AngularVelocity, Wheels[2].AngularVelocity - BrakeDecel, DeltaTime, 8.0f);
		Wheels[3].AngularVelocity = FMath::FInterpTo(Wheels[3].AngularVelocity, Wheels[3].AngularVelocity - BrakeDecel, DeltaTime, 8.0f);
	}
}

// ============================================================================
// DRIVER ASSISTANCE: ABS & TRACTION CONTROL (TCS)
// ============================================================================

void ABaseCar::ApplyABSAndTractionControl(float DeltaTime)
{
	// 1. Traction Control System (TCS)
	EffectiveThrottle = RawThrottle;
	Telemetry.bTCSActive = false;

	if (bRevLimiterCutting)
	{
		EffectiveThrottle = 0.0f;
	}
	else if (PhysicsConfig.Assistance.bTCSEnabled && RawThrottle > 0.05f)
	{
		// Monitor driven rear wheels slip
		float RearSlipMax = FMath::Max(Wheels[2].SlipRatio, Wheels[3].SlipRatio);
		if (RearSlipMax > PhysicsConfig.Assistance.TCSSlipThreshold)
		{
			Telemetry.bTCSActive = true;
			float SlipExcess = (RearSlipMax - PhysicsConfig.Assistance.TCSSlipThreshold);
			float CutAmount = FMath::Clamp(SlipExcess * 4.0f * PhysicsConfig.Assistance.TCSAggressiveness, 0.0f, 0.85f);
			EffectiveThrottle = RawThrottle * (1.0f - CutAmount);
		}
	}

	// 2. Anti-lock Braking System (ABS)
	EffectiveBrake = RawBrake;
	Telemetry.bABSActive = false;

	if (PhysicsConfig.Assistance.bABSEnabled && RawBrake > 0.1f)
	{
		bool bAnyWheelLockedOrSlipping = false;
		for (int32 i = 0; i < 4; ++i)
		{
			if (Wheels[i].SlipRatio < -PhysicsConfig.Assistance.ABSSlipThreshold || Wheels[i].bIsLocked)
			{
				bAnyWheelLockedOrSlipping = true;
				break;
			}
		}

		if (bAnyWheelLockedOrSlipping && Telemetry.SpeedKmh > 10.0f)
		{
			Telemetry.bABSActive = true;

			// Pulse ABS brake fluid pressure at configured frequency
			ABSPulseTimer += DeltaTime * PhysicsConfig.Assistance.ABSPulseFrequency;
			if (ABSPulseTimer >= 1.0f)
			{
				ABSPulseTimer = 0.0f;
				bABSPulseHigh = !bABSPulseHigh;
			}

			// Modulate pressure: dip when pulsing to allow tire to regain rotation
			EffectiveBrake = bABSPulseHigh ? RawBrake : RawBrake * 0.25f;
		}
	}
}

// ============================================================================
// AERODYNAMICS & ACTIVE AIRBRAKE
// ============================================================================

void ABaseCar::SimulateAerodynamics(float DeltaTime)
{
	float SpeedMps = Telemetry.SpeedKmh / 3.6f;
	float Rho = PhysicsConfig.Aerodynamics.AirDensity;
	float Area = PhysicsConfig.Aerodynamics.FrontalArea;
	float Cd = PhysicsConfig.Aerodynamics.DragCoefficient;
	float Cl = PhysicsConfig.Aerodynamics.DownforceCoefficient;

	// Active Aero / Airbrake Deployment check
	bool bDeployAirbrake = false;
	if (PhysicsConfig.Aerodynamics.bHasActiveAero)
	{
		// Deploy airbrake when braking hard from high speed (> 120 km/h)
		if (RawBrake > 0.45f && Telemetry.SpeedKmh > 120.0f)
		{
			bDeployAirbrake = true;
			Cd *= PhysicsConfig.Aerodynamics.AirbrakeDragMultiplier;
			Cl *= PhysicsConfig.Aerodynamics.AirbrakeDownforceMultiplier;
			ActiveAeroAngle = FMath::FInterpTo(ActiveAeroAngle, 55.0f, DeltaTime, 12.0f); // 55 degree high angle of attack
		}
		else
		{
			// Adaptive high speed downforce mode
			float TargetAngle = FMath::GetMappedRangeValueClamped(FVector2D(100.0f, 300.0f), FVector2D(5.0f, 25.0f), Telemetry.SpeedKmh);
			ActiveAeroAngle = FMath::FInterpTo(ActiveAeroAngle, TargetAngle, DeltaTime, 6.0f);
		}
	}

	Telemetry.bAirbrakeActive = bDeployAirbrake;

	// Drag Force: F_drag = 0.5 * rho * Cd * A * v^2
	float DragForceN = 0.5f * Rho * Cd * Area * SpeedMps * SpeedMps;

	// Downforce: F_down = 0.5 * rho * Cl * A * v^2
	float DownforceN = 0.5f * Rho * Cl * Area * SpeedMps * SpeedMps;

	Telemetry.TotalDragNewtons = DragForceN;
	Telemetry.TotalDownforceNewtons = DownforceN;

	// Distribute downforce across front and rear axles
	float FrontDownforce = DownforceN * PhysicsConfig.Aerodynamics.AeroBalanceFront * 0.5f;
	float RearDownforce = DownforceN * (1.0f - PhysicsConfig.Aerodynamics.AeroBalanceFront) * 0.5f;

	Wheels[0].VerticalLoadN += FrontDownforce;
	Wheels[1].VerticalLoadN += FrontDownforce;
	Wheels[2].VerticalLoadN += RearDownforce;
	Wheels[3].VerticalLoadN += RearDownforce;
}

// ============================================================================
// VISUAL PHYSICS & PRESENTATION
// ============================================================================

void ABaseCar::UpdateVisualWheelRotation(float DeltaTime)
{
	float SpeedMps = (GetVelocity().Size() * 0.01f);

	for (int32 i = 0; i < 4; ++i)
	{
		float WheelRadiusM = ((i < 2) ? PhysicsConfig.Tires.WheelRadiusFront : PhysicsConfig.Tires.WheelRadiusRear) * 0.01f;

		// If grounded and rolling, match speed; or integrate angular velocity
		float RadPerSec = Wheels[i].AngularVelocity;
		if (FMath::Abs(RadPerSec) < 0.01f && SpeedMps > 0.1f)
		{
			RadPerSec = SpeedMps / WheelRadiusM;
			Wheels[i].AngularVelocity = RadPerSec;
		}

		float RotDegrees = FMath::RadiansToDegrees(RadPerSec * DeltaTime);
		Wheels[i].VisualRotationDegrees = FMath::Fmod(Wheels[i].VisualRotationDegrees + RotDegrees, 360.0f);
	}
}

void ABaseCar::UpdateVisualSuspension(float DeltaTime)
{
	// Calculate suspension deflection based on dynamic vertical load vs equilibrium
	float StaticQuarterMass = (PhysicsConfig.TotalMassKg * 9.81f) * 0.25f;

	for (int32 i = 0; i < 4; ++i)
	{
		float SpringRate = (i < 2) ? PhysicsConfig.Suspension.SpringRateFront : PhysicsConfig.Suspension.SpringRateRear;
		float DeltaLoad = Wheels[i].VerticalLoadN - StaticQuarterMass;

		// Compression displacement x = F / k in centimeters
		float TargetDispCm = -(DeltaLoad / SpringRate) * 100.0f;
		TargetDispCm = FMath::Clamp(TargetDispCm, -PhysicsConfig.Suspension.SuspensionMaxDrop, PhysicsConfig.Suspension.SuspensionMaxRaise);

		float MaxRaise = PhysicsConfig.Suspension.SuspensionMaxRaise;
		if (MaxRaise > 0.0f && TargetDispCm >= MaxRaise * 0.90f)
		{
			OnSuspensionBottomOut.Broadcast(i, TargetDispCm / MaxRaise);
		}

		Wheels[i].SuspensionDisplacementCm = FMath::FInterpTo(Wheels[i].SuspensionDisplacementCm, TargetDispCm, DeltaTime, 15.0f);
	}
}

void ABaseCar::UpdateVisualLights(float DeltaTime)
{
	// Headlights
	if (HeadlightLeft && HeadlightRight)
	{
		HeadlightLeft->SetVisibility(bHeadlightsOn);
		HeadlightRight->SetVisibility(bHeadlightsOn);
	}

	// Reverse Backup Light
	if (ReverseLight)
	{
		ReverseLight->SetVisibility(CurrentGear == -1);
	}

	// Taillights & Dynamic Brake Strobe under emergency ABS
	float BaseIntensity = bHeadlightsOn ? 1800.0f : 400.0f;
	if (RawBrake > 0.05f)
	{
		if (Telemetry.bABSActive)
		{
			// Rapid flashing emergency brake strobe (standard in modern hypercars)
			float StrobeFactor = (FMath::Fmod(GetWorld()->GetTimeSeconds() * 12.0f, 1.0f) > 0.5f) ? 14000.0f : 1200.0f;
			if (TaillightLeft) TaillightLeft->SetIntensity(StrobeFactor);
			if (TaillightRight) TaillightRight->SetIntensity(StrobeFactor);
		}
		else
		{
			// Solid bright brake light
			float BrakeIntensity = FMath::Lerp(BaseIntensity, 8500.0f, RawBrake);
			if (TaillightLeft) TaillightLeft->SetIntensity(BrakeIntensity);
			if (TaillightRight) TaillightRight->SetIntensity(BrakeIntensity);
		}
	}
	else
	{
		if (TaillightLeft) TaillightLeft->SetIntensity(BaseIntensity);
		if (TaillightRight) TaillightRight->SetIntensity(BaseIntensity);
	}
}

void ABaseCar::UpdateAudioPitchAndVolume(float DeltaTime)
{
	if (!EngineAudioComponent)
	{
		return;
	}

	float Idle = PhysicsConfig.Engine.IdleRPM;
	float Redline = PhysicsConfig.Engine.RedlineRPM;

	// Pitch multiplier: 1.0 at idle, scales up to ~3.0 - 4.5 at redline
	float NormalizedRPM = (CurrentRPM - Idle) / (Redline - Idle);
	NormalizedRPM = FMath::Clamp(NormalizedRPM, 0.0f, 1.0f);

	float PitchMultiplier = FMath::Lerp(0.85f, 3.4f, NormalizedRPM);

	// Throttle load adds presence and volume
	float VolumeMultiplier = FMath::Lerp(0.55f, 1.25f, (NormalizedRPM * 0.65f) + (EffectiveThrottle * 0.35f));

	// Rev limiter staccato flutter
	if (bRevLimiterCutting)
	{
		VolumeMultiplier *= 0.6f;
		PitchMultiplier *= 1.02f;
	}

	EngineAudioComponent->SetPitchMultiplier(PitchMultiplier);
	EngineAudioComponent->SetVolumeMultiplier(VolumeMultiplier);
}
