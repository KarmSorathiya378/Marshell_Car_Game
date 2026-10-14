// Copyright VELOCITY X Racing. All Rights Reserved.

#include "AVelocityXCameraManager.h"
#include "AVelocityXCar.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AVelocityXCameraManager::AVelocityXCameraManager()
{
	PrimaryActorTick.bCanEverTick = true;

	// Root Scene Component
	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	SetRootComponent(RootComp);

	// Spring Arm Component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	if (SpringArm && RootComp)
	{
		SpringArm->SetupAttachment(RootComp);
		SpringArm->TargetArmLength = 450.0f;
		SpringArm->SocketOffset = FVector(0.0f, 0.0f, 125.0f);
		SpringArm->SetRelativeRotation(FRotator(-11.0f, 0.0f, 0.0f));
		SpringArm->bEnableCameraLag = true;
		SpringArm->CameraLagSpeed = 14.0f;
		SpringArm->bEnableCameraRotationLag = true;
		SpringArm->CameraRotationLagSpeed = 16.0f;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bInheritRoll = false;
	}

	// Main Camera Component
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	if (MainCamera && SpringArm)
	{
		MainCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
		MainCamera->FieldOfView = BaseFOV;
	}

	CurrentMode = EVelocityXCameraMode::Chase;
	CurrentFOV = BaseFOV;
	CurrentTargetArmLength = 450.0f;
	ShakeTimer = 0.0f;
}

void AVelocityXCameraManager::BeginPlay()
{
	Super::BeginPlay();

	// If no vehicle assigned, search for the first AVelocityXCar in world
	if (!TargetVehicle && GetWorld())
	{
		TArray<AActor*> FoundCars;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVelocityXCar::StaticClass(), FoundCars);
		if (FoundCars.Num() > 0)
		{
			SetTargetVehicle(Cast<AVelocityXCar>(FoundCars[0]));
		}
	}
}

void AVelocityXCameraManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!TargetVehicle || DeltaTime <= 0.0f)
	{
		return;
	}

	// 1. Update camera position and rotation per mode with smooth blending
	UpdateCameraModeTransform(DeltaTime);

	// 2. Update dynamic Field of View based on speed
	UpdateDynamicFOV(DeltaTime);
}

void AVelocityXCameraManager::SetTargetVehicle(AVelocityXCar* NewVehicle)
{
	TargetVehicle = NewVehicle;
	if (TargetVehicle)
	{
		SetActorLocation(TargetVehicle->GetActorLocation());
		SetActorRotation(TargetVehicle->GetActorRotation());
		SetCameraMode(CurrentMode, true);
		UE_LOG(LogTemp, Display, TEXT("[AVelocityXCameraManager] Target vehicle set: %s"), *TargetVehicle->GetName());
	}
}

void AVelocityXCameraManager::SetCameraMode(EVelocityXCameraMode NewMode, bool bSnapImmediate)
{
	CurrentMode = NewMode;

	switch (NewMode)
	{
	case EVelocityXCameraMode::Chase:
		CurrentTargetArmLength = 450.0f;
		if (SpringArm)
		{
			SpringArm->bEnableCameraLag = true;
			SpringArm->bEnableCameraRotationLag = true;
			SpringArm->SocketOffset = FVector(0.0f, 0.0f, 125.0f);
		}
		break;

	case EVelocityXCameraMode::Hood:
		CurrentTargetArmLength = 0.0f;
		if (SpringArm)
		{
			SpringArm->bEnableCameraLag = false;
			SpringArm->bEnableCameraRotationLag = false;
			SpringArm->SocketOffset = FVector::ZeroVector;
		}
		break;

	case EVelocityXCameraMode::Cockpit:
		CurrentTargetArmLength = 0.0f;
		if (SpringArm)
		{
			SpringArm->bEnableCameraLag = false;
			SpringArm->bEnableCameraRotationLag = false;
			SpringArm->SocketOffset = FVector::ZeroVector;
		}
		break;
	}

	if (bSnapImmediate && SpringArm && TargetVehicle)
	{
		SpringArm->TargetArmLength = CurrentTargetArmLength;
		FVector TargetLoc;
		FRotator TargetRot;

		if (CurrentMode == EVelocityXCameraMode::Chase)
		{
			TargetLoc = TargetVehicle->GetCameraSocketLocation(AVelocityXCar::SocketChaseBoom);
			TargetRot = TargetVehicle->GetActorRotation();
			TargetRot.Pitch = -11.0f;
		}
		else if (CurrentMode == EVelocityXCameraMode::Hood)
		{
			TargetLoc = TargetVehicle->GetCameraSocketLocation(AVelocityXCar::SocketHoodCamera);
			TargetRot = TargetVehicle->GetCameraSocketRotation(AVelocityXCar::SocketHoodCamera);
		}
		else
		{
			TargetLoc = TargetVehicle->GetCameraSocketLocation(AVelocityXCar::SocketCockpitCamera);
			TargetRot = TargetVehicle->GetCameraSocketRotation(AVelocityXCar::SocketCockpitCamera);
		}

		SetActorLocationAndRotation(TargetLoc, TargetRot);
	}

	UE_LOG(LogTemp, Log, TEXT("[AVelocityXCameraManager] Switched camera mode to: %d"), static_cast<int32>(NewMode));
}

void AVelocityXCameraManager::CycleCameraMode()
{
	switch (CurrentMode)
	{
	case EVelocityXCameraMode::Chase:
		SetCameraMode(EVelocityXCameraMode::Hood);
		break;
	case EVelocityXCameraMode::Hood:
		SetCameraMode(EVelocityXCameraMode::Cockpit);
		break;
	case EVelocityXCameraMode::Cockpit:
		SetCameraMode(EVelocityXCameraMode::Chase);
		break;
	}
}

void AVelocityXCameraManager::UpdateCameraModeTransform(float DeltaTime)
{
	if (!TargetVehicle || !SpringArm)
	{
		return;
	}

	// Smoothly interpolate arm length
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, CurrentTargetArmLength, DeltaTime, TransitionSpeed);

	FVector DesiredLoc = TargetVehicle->GetActorLocation();
	FRotator DesiredRot = TargetVehicle->GetActorRotation();

	switch (CurrentMode)
	{
	case EVelocityXCameraMode::Chase:
	{
		DesiredLoc = TargetVehicle->GetCameraSocketLocation(AVelocityXCar::SocketChaseBoom);
		DesiredRot = TargetVehicle->GetActorRotation();
		DesiredRot.Pitch = -11.0f;
		DesiredRot.Roll = 0.0f;

		// Speed-sensitive dynamic pullback
		float SpeedKph = TargetVehicle->GetSpeedKph();
		float PullbackExtra = FMath::GetMappedRangeValueClamped(FVector2D(50.0f, 350.0f), FVector2D(0.0f, 65.0f), SpeedKph);
		SpringArm->TargetArmLength = 450.0f + PullbackExtra;
		break;
	}

	case EVelocityXCameraMode::Hood:
	{
		DesiredLoc = TargetVehicle->GetCameraSocketLocation(AVelocityXCar::SocketHoodCamera);
		DesiredRot = TargetVehicle->GetCameraSocketRotation(AVelocityXCar::SocketHoodCamera);
		break;
	}

	case EVelocityXCameraMode::Cockpit:
	{
		DesiredLoc = TargetVehicle->GetCameraSocketLocation(AVelocityXCar::SocketCockpitCamera);
		DesiredRot = TargetVehicle->GetCameraSocketRotation(AVelocityXCar::SocketCockpitCamera);

		// Look slightly into corners (yaw deflection based on lateral G)
		float TurnYawOffset = -TargetVehicle->GetLateralG() * 6.5f;
		DesiredRot.Yaw += TurnYawOffset;
		break;
	}
	}

	// High speed camera shake injection
	ApplyHighSpeedShake(DeltaTime, DesiredLoc, DesiredRot);

	// Smoothly blend camera root actor to vehicle location and orientation
	FVector NewLoc = FMath::VInterpTo(GetActorLocation(), DesiredLoc, DeltaTime, TransitionSpeed * 1.5f);
	FRotator NewRot = FMath::RInterpTo(GetActorRotation(), DesiredRot, DeltaTime, TransitionSpeed * 1.5f);

	SetActorLocationAndRotation(NewLoc, NewRot);
}

void AVelocityXCameraManager::UpdateDynamicFOV(float DeltaTime)
{
	if (!MainCamera || !TargetVehicle)
	{
		return;
	}

	float SpeedKph = TargetVehicle->GetSpeedKph();

	// Smoothly scale FOV from BaseFOV to MaxFOV based on speed
	float TargetFOV = BaseFOV;
	if (SpeedKph > 40.0f)
	{
		float SpeedFraction = FMath::Clamp((SpeedKph - 40.0f) / (SpeedForMaxFOVKph - 40.0f), 0.0f, 1.0f);
		// Non-linear easing to emphasize dramatic rush at high speeds
		float EasedFraction = SpeedFraction * SpeedFraction;
		TargetFOV = FMath::Lerp(BaseFOV, MaxFOV, EasedFraction);
	}

	CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, 6.0f);
	MainCamera->SetFieldOfView(CurrentFOV);
}

void AVelocityXCameraManager::ApplyHighSpeedShake(float DeltaTime, FVector& OutLocation, FRotator& OutRotation)
{
	if (!bEnableHighSpeedShake || !TargetVehicle)
	{
		return;
	}

	float SpeedKph = TargetVehicle->GetSpeedKph();
	if (SpeedKph > HighSpeedShakeThresholdKph)
	{
		ShakeTimer += DeltaTime * 22.0f; // High frequency vibration

		float SpeedFactor = FMath::Clamp((SpeedKph - HighSpeedShakeThresholdKph) / (380.0f - HighSpeedShakeThresholdKph), 0.0f, 1.0f);
		float Intensity = SpeedFactor * MaxShakeIntensity;

		// Multi-frequency trigonometric shake
		float ShakePitch = FMath::Sin(ShakeTimer * 1.15f) * Intensity * 0.45f;
		float ShakeYaw = FMath::Cos(ShakeTimer * 0.92f) * Intensity * 0.35f;
		float ShakeRoll = FMath::Sin(ShakeTimer * 1.48f) * Intensity * 0.25f;

		OutRotation.Pitch += ShakePitch;
		OutRotation.Yaw += ShakeYaw;
		OutRotation.Roll += ShakeRoll;

		float ShakeZ = FMath::Cos(ShakeTimer * 2.1f) * Intensity * 0.8f;
		OutLocation.Z += ShakeZ;
	}
}
