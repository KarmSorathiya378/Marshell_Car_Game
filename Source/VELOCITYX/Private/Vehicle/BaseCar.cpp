// Copyright VELOCITY X Racing. All Rights Reserved.

#include "Vehicle/BaseCar.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ABaseCar::ABaseCar()
{
	PrimaryActorTick.bCanEverTick = true;

	// Spring arm chase boom
	ChaseCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("ChaseCameraBoom"));
	ChaseCameraBoom->SetupAttachment(RootComponent);
	ChaseCameraBoom->TargetArmLength = 450.0f;
	ChaseCameraBoom->SocketOffset = FVector(0.0f, 0.0f, 130.0f);
	ChaseCameraBoom->SetRelativeRotation(FRotator(-10.0f, 0.0f, 0.0f));
	ChaseCameraBoom->bInheritPitch = true;
	ChaseCameraBoom->bInheritYaw = true;
	ChaseCameraBoom->bInheritRoll = false;
	ChaseCameraBoom->bEnableCameraLag = true;
	ChaseCameraBoom->CameraLagSpeed = 12.0f;

	// Chase camera
	ChaseCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	ChaseCamera->SetupAttachment(ChaseCameraBoom, USpringArmComponent::SocketName);
	ChaseCamera->bUsePawnControlRotation = false;

	// Cockpit / Bonnet camera
	CockpitCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CockpitCamera"));
	CockpitCamera->SetupAttachment(RootComponent);
	CockpitCamera->SetRelativeLocation(FVector(20.0f, -30.0f, 110.0f)); // Driver seat offset
	CockpitCamera->bUsePawnControlRotation = false;
	CockpitCamera->SetActive(false);

	bHeadlightsOn = false;
	ActiveCameraIndex = 0;
	CurrentForwardSpeed = 0.0f;

	SetupCarDefaults();
}

void ABaseCar::SetupCarDefaults()
{
	PerformanceStats.ModelName = TEXT("Base Car Platform");
	PerformanceStats.Horsepower = 500.0f;
	PerformanceStats.WeightKg = 1450.0f;
	PerformanceStats.MaxSpeedMPH = 195.0f;
	PerformanceStats.Acceleration0to60 = 3.5f;
	PerformanceStats.BrakeForce = 4000.0f;
	PerformanceStats.SteerSensitivity = 1.0f;
	PerformanceStats.GripFactor = 1.0f;
	PerformanceStats.DragCoefficient = 0.31f;
	PerformanceStats.DownforceMultiplier = 1.0f;
	PerformanceStats.NumGears = 6;
	PerformanceStats.EngineTorque = 600.0f;
	PerformanceStats.RedlineRPM = 7500.0f;
}

void ABaseCar::BeginPlay()
{
	Super::BeginPlay();

	ChaosVehicleComponent = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent());
	if (!ChaosVehicleComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChaosWheeledVehicleMovementComponent not found on BaseCar."));
	}
}

void ABaseCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentForwardSpeed = GetVelocity().Size();
}

void ABaseCar::ApplyThrottle(float Value)
{
	if (ChaosVehicleComponent)
	{
		ChaosVehicleComponent->SetThrottleInput(FMath::Clamp(Value, 0.0f, 1.0f));
	}
}

void ABaseCar::ApplySteering(float Value)
{
	if (ChaosVehicleComponent)
	{
		ChaosVehicleComponent->SetSteeringInput(FMath::Clamp(Value, -1.0f, 1.0f) * PerformanceStats.SteerSensitivity);
	}
}

void ABaseCar::ApplyBrake(float Value)
{
	if (ChaosVehicleComponent)
	{
		ChaosVehicleComponent->SetBrakeInput(FMath::Clamp(Value, 0.0f, 1.0f));
	}
}

void ABaseCar::ApplyHandbrake(bool bHandbrake)
{
	if (ChaosVehicleComponent)
	{
		ChaosVehicleComponent->SetHandbrakeInput(bHandbrake);
	}
}

void ABaseCar::ResetVehicle()
{
	if (ChaosVehicleComponent)
	{
		ChaosVehicleComponent->ResetVehicleState();
	}

	FVector CurrentLocation = GetActorLocation();
	FRotator CurrentRotation = GetActorRotation();
	CurrentRotation.Roll = 0.0f;
	CurrentRotation.Pitch = 0.0f;

	SetActorLocationAndRotation(CurrentLocation + FVector(0.0f, 0.0f, 50.0f), CurrentRotation, false, nullptr, ETeleportType::TeleportPhysics);

	UE_LOG(LogTemp, Display, TEXT("%s reset upright."), *PerformanceStats.ModelName);
}

void ABaseCar::ToggleHeadlights()
{
	bHeadlightsOn = !bHeadlightsOn;
	UE_LOG(LogTemp, Display, TEXT("[%s] Headlights set to: %s"), *PerformanceStats.ModelName, bHeadlightsOn ? TEXT("ON") : TEXT("OFF"));
}

void ABaseCar::CycleCameraMode()
{
	ActiveCameraIndex = (ActiveCameraIndex + 1) % 2;

	if (ActiveCameraIndex == 0)
	{
		ChaseCamera->SetActive(true);
		CockpitCamera->SetActive(false);
	}
	else
	{
		ChaseCamera->SetActive(false);
		CockpitCamera->SetActive(true);
	}

	UE_LOG(LogTemp, Display, TEXT("[%s] Switched camera mode to index: %d"), *PerformanceStats.ModelName, ActiveCameraIndex);
}

float ABaseCar::GetSpeedMPH() const
{
	// Velocity is in cm/s -> convert to MPH (1 cm/s = 0.0223694 MPH)
	return GetVelocity().Size() * 0.0223694f;
}

float ABaseCar::GetSpeedKPH() const
{
	// Velocity is in cm/s -> convert to km/h (1 cm/s = 0.036 km/h)
	return GetVelocity().Size() * 0.036f;
}

float ABaseCar::GetEngineRPM() const
{
	if (ChaosVehicleComponent)
	{
		return ChaosVehicleComponent->GetEngineRotationSpeed();
	}
	return 0.0f;
}

int32 ABaseCar::GetCurrentGear() const
{
	if (ChaosVehicleComponent)
	{
		return ChaosVehicleComponent->GetCurrentGear();
	}
	return 0;
}
