// Copyright VELOCITY X Racing. All Rights Reserved.

#include "AVelocityXCar.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

// Static Socket Names
const FName AVelocityXCar::SocketCockpitCamera = TEXT("Socket_CockpitCamera");
const FName AVelocityXCar::SocketHoodCamera = TEXT("Socket_HoodCamera");
const FName AVelocityXCar::SocketChaseBoom = TEXT("Socket_ChaseBoom");
const FName AVelocityXCar::SocketExhaustLeft = TEXT("Socket_Exhaust_L");
const FName AVelocityXCar::SocketExhaustRight = TEXT("Socket_Exhaust_R");

AVelocityXCar::AVelocityXCar()
{
	PrimaryActorTick.bCanEverTick = true;

	// Headlights (Spotlights)
	HeadlightLeft = CreateDefaultSubobject<USpotLightComponent>(TEXT("HeadlightLeft"));
	if (HeadlightLeft && RootComponent)
	{
		HeadlightLeft->SetupAttachment(RootComponent);
		HeadlightLeft->SetRelativeLocation(FVector(190.0f, -65.0f, 35.0f));
		HeadlightLeft->SetRelativeRotation(FRotator(-3.0f, 0.0f, 0.0f));
		HeadlightLeft->Intensity = 45000.0f;
		HeadlightLeft->AttenuationRadius = 4000.0f;
		HeadlightLeft->InnerConeAngle = 18.0f;
		HeadlightLeft->OuterConeAngle = 36.0f;
		HeadlightLeft->SetVisibility(false);
	}

	HeadlightRight = CreateDefaultSubobject<USpotLightComponent>(TEXT("HeadlightRight"));
	if (HeadlightRight && RootComponent)
	{
		HeadlightRight->SetupAttachment(RootComponent);
		HeadlightRight->SetRelativeLocation(FVector(190.0f, 65.0f, 35.0f));
		HeadlightRight->SetRelativeRotation(FRotator(-3.0f, 0.0f, 0.0f));
		HeadlightRight->Intensity = 45000.0f;
		HeadlightRight->AttenuationRadius = 4000.0f;
		HeadlightRight->InnerConeAngle = 18.0f;
		HeadlightRight->OuterConeAngle = 36.0f;
		HeadlightRight->SetVisibility(false);
	}

	// Taillights (Red Point Lights)
	TaillightLeft = CreateDefaultSubobject<UPointLightComponent>(TEXT("TaillightLeft"));
	if (TaillightLeft && RootComponent)
	{
		TaillightLeft->SetupAttachment(RootComponent);
		TaillightLeft->SetRelativeLocation(FVector(-190.0f, -65.0f, 40.0f));
		TaillightLeft->SetLightColor(FLinearColor(1.0f, 0.02f, 0.02f));
		TaillightLeft->Intensity = 2000.0f;
		TaillightLeft->AttenuationRadius = 600.0f;
	}

	TaillightRight = CreateDefaultSubobject<UPointLightComponent>(TEXT("TaillightRight"));
	if (TaillightRight && RootComponent)
	{
		TaillightRight->SetupAttachment(RootComponent);
		TaillightRight->SetRelativeLocation(FVector(-190.0f, 65.0f, 40.0f));
		TaillightRight->SetLightColor(FLinearColor(1.0f, 0.02f, 0.02f));
		TaillightRight->Intensity = 2000.0f;
		TaillightRight->AttenuationRadius = 600.0f;
	}

	// Reverse Backup Light (White Point Light)
	ReverseLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ReverseLight"));
	if (ReverseLight && RootComponent)
	{
		ReverseLight->SetupAttachment(RootComponent);
		ReverseLight->SetRelativeLocation(FVector(-195.0f, 0.0f, 30.0f));
		ReverseLight->SetLightColor(FLinearColor(1.0f, 1.0f, 0.95f));
		ReverseLight->Intensity = 2800.0f;
		ReverseLight->AttenuationRadius = 600.0f;
		ReverseLight->SetVisibility(false);
	}

	// 4 Rotating Wheel Mesh Components
	static const TCHAR* WheelNames[4] = {
		TEXT("Wheel_FrontLeft"),
		TEXT("Wheel_FrontRight"),
		TEXT("Wheel_RearLeft"),
		TEXT("Wheel_RearRight")
	};

	FVector WheelOffsets[4] = {
		FVector(135.0f, -85.0f, 0.0f),  // FL
		FVector(135.0f, 85.0f, 0.0f),   // FR
		FVector(-135.0f, -85.0f, 0.0f), // RL
		FVector(-135.0f, 85.0f, 0.0f)   // RR
	};

	for (int32 i = 0; i < 4; ++i)
	{
		WheelMeshComponents[i] = CreateDefaultSubobject<UStaticMeshComponent>(FName(WheelNames[i]));
		if (WheelMeshComponents[i] && RootComponent)
		{
			WheelMeshComponents[i]->SetupAttachment(RootComponent);
			WheelMeshComponents[i]->SetRelativeLocation(WheelOffsets[i]);
			WheelMeshComponents[i]->SetCollisionProfileName(TEXT("NoCollision"));
		}
	}

	bHeadlightsOn = false;

	// Apply Supercar preset by default
	ApplyPreset(EVelocityXCarPreset::Supercar);
}

void AVelocityXCar::BeginPlay()
{
	Super::BeginPlay();
	ApplyPreset(CurrentPreset);
}

void AVelocityXCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update visual wheel pitch rotation and front steer yaw
	UpdateVisualWheels(DeltaTime);

	// Update headlights, taillights, and brake lights
	UpdateLighting(DeltaTime);
}

void AVelocityXCar::ApplyPreset(EVelocityXCarPreset InPreset)
{
	CurrentPreset = InPreset;

	switch (InPreset)
	{
	case EVelocityXCarPreset::Supercar:
	{
		ActiveStats.CarName = TEXT("Apex GT3-R (Supercar)");
		ActiveStats.MaxSpeedKmh = 320.0f;
		ActiveStats.Acceleration0To100Seconds = 3.5f;
		ActiveStats.HandlingRating = 88.0f;
		ActiveStats.BrakingRating = 86.0f;
		ActiveStats.Horsepower = 500.0f;
		ActiveStats.PeakTorqueNm = 470.0f;
		ActiveStats.WeightKg = 1500.0f;
		ActiveStats.DownforceCoefficient = 1.85f;
		ActiveStats.bIsAWD = false;

		// Chaos vehicle physics parameters
		Horsepower = 500.0f;
		PeakTorqueNm = 470.0f;
		IdleRPM = 900.0f;
		RedlineRPM = 9000.0f; // Naturally aspirated scream
		ThrottleResponseRate = 22.0f;
		VehicleMassKg = 1500.0f;
		FinalDriveRatio = 3.97f;

		// Stiff sports suspension
		WheelsConfig[0].SuspensionStiffness = 85000.0f;
		WheelsConfig[1].SuspensionStiffness = 85000.0f;
		WheelsConfig[2].SuspensionStiffness = 120000.0f;
		WheelsConfig[3].SuspensionStiffness = 120000.0f;
		WheelsConfig[0].FrictionMultiplier = 1.68f;
		WheelsConfig[1].FrictionMultiplier = 1.68f;
		WheelsConfig[2].FrictionMultiplier = 1.75f;
		WheelsConfig[3].FrictionMultiplier = 1.75f;
		break;
	}

	case EVelocityXCarPreset::TrackSupercar:
	{
		ActiveStats.CarName = TEXT("Veloce Senna-R (Track Supercar)");
		ActiveStats.MaxSpeedKmh = 355.0f;
		ActiveStats.Acceleration0To100Seconds = 2.8f;
		ActiveStats.HandlingRating = 98.0f; // Track precision
		ActiveStats.BrakingRating = 95.0f;
		ActiveStats.Horsepower = 720.0f;
		ActiveStats.PeakTorqueNm = 800.0f;
		ActiveStats.WeightKg = 1200.0f; // Ultralight carbon tub
		ActiveStats.DownforceCoefficient = 2.65f;
		ActiveStats.bIsAWD = false;

		Horsepower = 720.0f;
		PeakTorqueNm = 800.0f;
		IdleRPM = 950.0f;
		RedlineRPM = 8500.0f;
		ThrottleResponseRate = 25.0f;
		VehicleMassKg = 1200.0f;
		FinalDriveRatio = 3.73f;

		// Race suspension
		WheelsConfig[0].SuspensionStiffness = 110000.0f;
		WheelsConfig[1].SuspensionStiffness = 110000.0f;
		WheelsConfig[2].SuspensionStiffness = 135000.0f;
		WheelsConfig[3].SuspensionStiffness = 135000.0f;
		WheelsConfig[0].FrictionMultiplier = 1.88f;
		WheelsConfig[1].FrictionMultiplier = 1.88f;
		WheelsConfig[2].FrictionMultiplier = 1.95f;
		WheelsConfig[3].FrictionMultiplier = 1.95f;
		break;
	}

	case EVelocityXCarPreset::Hypercar:
	{
		ActiveStats.CarName = TEXT("Titan Chiron-X (Hypercar)");
		ActiveStats.MaxSpeedKmh = 420.0f;
		ActiveStats.Acceleration0To100Seconds = 2.4f; // AWD catapult
		ActiveStats.HandlingRating = 85.0f;
		ActiveStats.BrakingRating = 96.0f;
		ActiveStats.Horsepower = 1500.0f;
		ActiveStats.PeakTorqueNm = 1600.0f;
		ActiveStats.WeightKg = 1900.0f;
		ActiveStats.DownforceCoefficient = 2.15f;
		ActiveStats.bIsAWD = true;

		Horsepower = 1500.0f;
		PeakTorqueNm = 1600.0f;
		IdleRPM = 850.0f;
		RedlineRPM = 7000.0f;
		ThrottleResponseRate = 18.0f;
		VehicleMassKg = 1900.0f;
		FinalDriveRatio = 3.20f;

		// AWD drive configuration
		WheelsConfig[0].bDriven = true;
		WheelsConfig[1].bDriven = true;
		WheelsConfig[2].bDriven = true;
		WheelsConfig[3].bDriven = true;

		WheelsConfig[0].SuspensionStiffness = 95000.0f;
		WheelsConfig[1].SuspensionStiffness = 95000.0f;
		WheelsConfig[2].SuspensionStiffness = 125000.0f;
		WheelsConfig[3].SuspensionStiffness = 125000.0f;
		WheelsConfig[0].FrictionMultiplier = 1.72f;
		WheelsConfig[1].FrictionMultiplier = 1.72f;
		WheelsConfig[2].FrictionMultiplier = 1.82f;
		WheelsConfig[3].FrictionMultiplier = 1.82f;
		break;
	}
	}

	UE_LOG(LogTemp, Display, TEXT("[VelocityXCar] Applied preset '%s': %d HP, Top Speed: %.0f km/h, 0-100: %.1fs"),
		*ActiveStats.CarName, FMath::RoundToInt(ActiveStats.Horsepower), ActiveStats.MaxSpeedKmh, ActiveStats.Acceleration0To100Seconds);
}

void AVelocityXCar::ToggleHeadlights()
{
	SetHeadlights(!bHeadlightsOn);
}

void AVelocityXCar::SetHeadlights(bool bEnable)
{
	bHeadlightsOn = bEnable;
	if (HeadlightLeft) HeadlightLeft->SetVisibility(bHeadlightsOn);
	if (HeadlightRight) HeadlightRight->SetVisibility(bHeadlightsOn);
	UE_LOG(LogTemp, Log, TEXT("[VelocityXCar] Headlights: %s"), bHeadlightsOn ? TEXT("ON") : TEXT("OFF"));
}

void AVelocityXCar::UpdateVisualWheels(float DeltaTime)
{
	for (int32 i = 0; i < 4; ++i)
	{
		if (WheelMeshComponents[i])
		{
			// Pitch rotation around axle based on rolling motion
			float PitchAngle = WheelsState[i].RotationDegrees;
			// Yaw steering angle for front steerable wheels
			float YawAngle = WheelsState[i].SteerAngleDegrees;

			FRotator WheelRot = FRotator(PitchAngle, YawAngle, 0.0f);
			WheelMeshComponents[i]->SetRelativeRotation(WheelRot);

			// Suspension vertical displacement
			FVector RelLoc = WheelMeshComponents[i]->GetRelativeLocation();
			RelLoc.Z = WheelsState[i].SuspensionCompressionCm;
			WheelMeshComponents[i]->SetRelativeLocation(RelLoc);
		}
	}
}

void AVelocityXCar::UpdateLighting(float DeltaTime)
{
	// Reverse Light
	if (ReverseLight)
	{
		ReverseLight->SetVisibility(CurrentGear == -1);
	}

	// Taillights and Brake Lights intensification
	float BaseIntensity = bHeadlightsOn ? 1800.0f : 400.0f;
	if (RawBrake > 0.05f)
	{
		// Brighten under braking
		float BrakeIntensity = FMath::Lerp(BaseIntensity, 8000.0f, RawBrake);
		if (TaillightLeft) TaillightLeft->SetIntensity(BrakeIntensity);
		if (TaillightRight) TaillightRight->SetIntensity(BrakeIntensity);
	}
	else
	{
		if (TaillightLeft) TaillightLeft->SetIntensity(BaseIntensity);
		if (TaillightRight) TaillightRight->SetIntensity(BaseIntensity);
	}
}

FVector AVelocityXCar::GetCameraSocketLocation(FName SocketName) const
{
	FVector ActorLoc = GetActorLocation();
	FVector Fwd = GetActorForwardVector();
	FVector Right = GetActorRightVector();
	FVector Up = GetActorUpVector();

	if (SocketName == SocketCockpitCamera)
	{
		// Driver head position inside cabin (left-hand drive)
		return ActorLoc + (Fwd * -15.0f) + (Right * -38.0f) + (Up * 62.0f);
	}
	else if (SocketName == SocketHoodCamera)
	{
		// Front hood edge position
		return ActorLoc + (Fwd * 145.0f) + (Up * 42.0f);
	}
	else if (SocketName == SocketChaseBoom)
	{
		// Chase camera pivot
		return ActorLoc + (Up * 80.0f);
	}
	else if (SocketName == SocketExhaustLeft)
	{
		return ActorLoc + (Fwd * -190.0f) + (Right * -35.0f) + (Up * 15.0f);
	}
	else if (SocketName == SocketExhaustRight)
	{
		return ActorLoc + (Fwd * -190.0f) + (Right * 35.0f) + (Up * 15.0f);
	}

	return ActorLoc;
}

FRotator AVelocityXCar::GetCameraSocketRotation(FName SocketName) const
{
	FRotator ActorRot = GetActorRotation();

	if (SocketName == SocketCockpitCamera)
	{
		return ActorRot;
	}
	else if (SocketName == SocketHoodCamera)
	{
		// Slight downward angle on hood camera
		FRotator R = ActorRot;
		R.Pitch -= 2.0f;
		return R;
	}

	return ActorRot;
}
