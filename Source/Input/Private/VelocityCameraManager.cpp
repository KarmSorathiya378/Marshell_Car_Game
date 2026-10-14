// Copyright VELOCITY X Racing. All Rights Reserved.

#include "VelocityCameraManager.h"
#include "VELOCITYXVehicle.h"
#include "Vehicle/BaseCar.h"
#include "VelocityCameraShake.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "CollisionQueryParams.h"
#include "Kismet/KismetMathLibrary.h"

AVelocityCameraManager::AVelocityCameraManager()
{
	// Chase Close Preset (Standard dynamic follow)
	ChaseClosePreset.RelativeOffset = FVector(-490.0f, 0.0f, 150.0f);
	ChaseClosePreset.LookAtOffset = FVector(80.0f, 0.0f, 45.0f);
	ChaseClosePreset.BaseFOV = 90.0f;
	ChaseClosePreset.PositionLagSpeed = 11.0f;
	ChaseClosePreset.RotationLagSpeed = 9.0f;
	ChaseClosePreset.SpeedPullbackDistance = 120.0f;
	ChaseClosePreset.bEnableCollision = true;
	ChaseClosePreset.bFollowYawOnly = false;
	ChaseClosePreset.PreferredMeshSocket = TEXT("Socket_ChaseCamera");

	// Chase Far Preset (Elevated, high-speed overview)
	ChaseFarPreset.RelativeOffset = FVector(-720.0f, 0.0f, 260.0f);
	ChaseFarPreset.LookAtOffset = FVector(120.0f, 0.0f, 60.0f);
	ChaseFarPreset.BaseFOV = 88.0f;
	ChaseFarPreset.PositionLagSpeed = 8.0f;
	ChaseFarPreset.RotationLagSpeed = 7.0f;
	ChaseFarPreset.SpeedPullbackDistance = 180.0f;
	ChaseFarPreset.bEnableCollision = true;
	ChaseFarPreset.bFollowYawOnly = false;
	ChaseFarPreset.PreferredMeshSocket = TEXT("Socket_ChaseFar");

	// Hood Preset (Driver forward hood view)
	HoodPreset.RelativeOffset = FVector(135.0f, 0.0f, 75.0f);
	HoodPreset.LookAtOffset = FVector(600.0f, 0.0f, 75.0f);
	HoodPreset.BaseFOV = 95.0f;
	HoodPreset.PositionLagSpeed = 28.0f;
	HoodPreset.RotationLagSpeed = 26.0f;
	HoodPreset.SpeedPullbackDistance = 0.0f;
	HoodPreset.bEnableCollision = false;
	HoodPreset.bFollowYawOnly = false;
	HoodPreset.PreferredMeshSocket = TEXT("Socket_HoodCamera");

	// Cockpit Preset (Interior cabin perspective)
	CockpitPreset.RelativeOffset = FVector(-12.0f, -34.0f, 96.0f);
	CockpitPreset.LookAtOffset = FVector(500.0f, -34.0f, 96.0f);
	CockpitPreset.BaseFOV = 90.0f;
	CockpitPreset.PositionLagSpeed = 32.0f;
	CockpitPreset.RotationLagSpeed = 32.0f;
	CockpitPreset.SpeedPullbackDistance = 0.0f;
	CockpitPreset.bEnableCollision = false;
	CockpitPreset.bFollowYawOnly = false;
	CockpitPreset.PreferredMeshSocket = TEXT("Socket_CockpitCamera");

	// Blending
	BlendDuration = 0.45f;
	bIsBlending = false;
	BlendAlpha = 1.0f;
	BlendTimer = 0.0f;

	// Speed FOV scaling (smooth widen from 90 at 0 mph up to 115-120 at 250+ mph)
	BaseMinFOV = 90.0f;
	MaxSpeedFOV = 118.0f;
	ReferenceMaxSpeedMPH = 250.0f;
	FOVInterpSpeed = 4.5f;

	// Motion Blur
	MinMotionBlurAmount = 0.15f;
	MaxMotionBlurAmount = 0.85f;
	bEnableMotionBlurEffect = true;

	// Collision
	CollisionProbeRadius = 18.0f;
	CollisionRecoverySpeed = 8.0f;
	CameraCollisionChannel = ECC_Camera;
	CurrentCollisionDistanceAlpha = 1.0f;

	// Shake
	bEnableHighSpeedShake = true;
	ShakeSpeedThresholdMPH = 60.0f;
	MaxShakePitchYawRoll = 0.75f;
	ShakeFrequency = 22.0f;
	ShakeTimer = 0.0f;
	ExternalWindShakeIntensity = 0.0f;
	ExternalRedlineShakeIntensity = 0.0f;

	HighSpeedShakeClass = UVelocityHighSpeedCameraShake::StaticClass();
	ImpactShakeClass = UVelocityImpactCameraShake::StaticClass();

	CurrentCameraMode = EVelocityCameraMode::ChaseClose;
	PreviousCameraMode = EVelocityCameraMode::ChaseClose;
	SmoothedFOV = 90.0f;
}

void AVelocityCameraManager::BeginPlay()
{
	Super::BeginPlay();

	SmoothedCameraLocation = GetCameraLocation();
	SmoothedCameraRotation = GetCameraRotation();
	SmoothedFOV = GetPresetForMode(CurrentCameraMode).BaseFOV;
}

void AVelocityCameraManager::SetTargetVehicle(AVELOCITYXVehicle* InVehicle)
{
	TargetVelocityVehicle = InVehicle;
	SetTargetPawn(InVehicle);
}

void AVelocityCameraManager::SetTargetBaseCar(ABaseCar* InCar)
{
	TargetBaseCar = InCar;
	SetTargetPawn(InCar);
}

void AVelocityCameraManager::SetTargetPawn(APawn* InPawn)
{
	TargetPawn = InPawn;
	if (TargetPawn)
	{
		if (!TargetVelocityVehicle)
		{
			TargetVelocityVehicle = Cast<AVELOCITYXVehicle>(TargetPawn);
		}
		if (!TargetBaseCar)
		{
			TargetBaseCar = Cast<ABaseCar>(TargetPawn);
		}
		ResetCamera();
	}
}

void AVelocityCameraManager::CycleCameraMode()
{
	EVelocityCameraMode NextMode = EVelocityCameraMode::ChaseClose;
	switch (CurrentCameraMode)
	{
	case EVelocityCameraMode::ChaseClose:
		NextMode = EVelocityCameraMode::Hood;
		break;
	case EVelocityCameraMode::Hood:
		NextMode = EVelocityCameraMode::Cockpit;
		break;
	case EVelocityCameraMode::Cockpit:
		NextMode = EVelocityCameraMode::ChaseFar;
		break;
	case EVelocityCameraMode::ChaseFar:
	case EVelocityCameraMode::Top:
		NextMode = EVelocityCameraMode::ChaseClose;
		break;
	}

	SetCameraMode(NextMode, false);
}

void AVelocityCameraManager::SetCameraMode(EVelocityCameraMode NewMode, bool bInstant)
{
	if (NewMode == CurrentCameraMode && !bInstant)
	{
		return;
	}

	PreviousCameraMode = CurrentCameraMode;
	CurrentCameraMode = NewMode;

	if (bInstant || BlendDuration <= KINDA_SMALL_NUMBER)
	{
		bIsBlending = false;
		BlendAlpha = 1.0f;
		BlendTimer = 0.0f;
		ResetCamera();
	}
	else
	{
		bIsBlending = true;
		BlendAlpha = 0.0f;
		BlendTimer = 0.0f;
		BlendStartLocation = SmoothedCameraLocation;
		BlendStartRotation = SmoothedCameraRotation;
		BlendStartFOV = SmoothedFOV;
	}

	UE_LOG(LogTemp, Display, TEXT("[VelocityCameraManager] Switched Camera Mode to %d (Instant: %s)"),
		static_cast<int32>(CurrentCameraMode), bInstant ? TEXT("true") : TEXT("false"));
}

void AVelocityCameraManager::ResetCamera()
{
	if (!TargetPawn && PCOwner)
	{
		SetTargetPawn(PCOwner->GetPawn());
	}

	if (TargetPawn)
	{
		FVector IdealLoc;
		FRotator IdealRot;
		float IdealFOV;
		ComputeModeTransform(CurrentCameraMode, TargetPawn, IdealLoc, IdealRot, IdealFOV);

		SmoothedCameraLocation = IdealLoc;
		SmoothedCameraRotation = IdealRot;
		SmoothedFOV = IdealFOV;
		bIsBlending = false;
		BlendAlpha = 1.0f;
		CurrentCollisionDistanceAlpha = 1.0f;
	}
}

void AVelocityCameraManager::TriggerCollisionShake(float ImpactMagnitude)
{
	TriggerCrashShake(ImpactMagnitude);
}

void AVelocityCameraManager::TriggerCrashShake(float Intensity)
{
	if (ImpactShakeClass)
	{
		StartCameraShake(ImpactShakeClass, FMath::Clamp(Intensity, 0.5f, 5.0f));
	}
}

void AVelocityCameraManager::TriggerHighSpeedWindShake(float WindIntensity)
{
	ExternalWindShakeIntensity = FMath::Clamp(WindIntensity, 0.0f, 2.0f);
}

void AVelocityCameraManager::TriggerEngineRedlineShake(float RPMPercent)
{
	ExternalRedlineShakeIntensity = FMath::Clamp(RPMPercent, 0.0f, 2.0f);
}

void AVelocityCameraManager::SetSpeedFOVRange(float InMinFOV, float InMaxFOV, float InMaxSpeedMPH)
{
	BaseMinFOV = InMinFOV;
	MaxSpeedFOV = InMaxFOV;
	ReferenceMaxSpeedMPH = FMath::Max(50.0f, InMaxSpeedMPH);
}

const FVelocityCameraPreset& AVelocityCameraManager::GetPresetForMode(EVelocityCameraMode Mode) const
{
	switch (Mode)
	{
	case EVelocityCameraMode::Hood:
		return HoodPreset;
	case EVelocityCameraMode::Cockpit:
		return CockpitPreset;
	case EVelocityCameraMode::ChaseFar:
	case EVelocityCameraMode::Top:
		return ChaseFarPreset;
	case EVelocityCameraMode::ChaseClose:
	default:
		return ChaseClosePreset;
	}
}

float AVelocityCameraManager::ResolveVehicleSpeedMPH() const
{
	if (TargetBaseCar)
	{
		return TargetBaseCar->GetSpeedMPH();
	}
	if (TargetVelocityVehicle)
	{
		return TargetVelocityVehicle->GetSpeedMPH();
	}
	if (TargetPawn)
	{
		return TargetPawn->GetVelocity().Size() * 0.0223694f; // cm/s to mph
	}
	return 0.0f;
}

void AVelocityCameraManager::ComputeModeTransform(EVelocityCameraMode Mode, const APawn* Pawn, FVector& OutLocation, FRotator& OutRotation, float& OutFOV) const
{
	if (!Pawn)
	{
		OutLocation = SmoothedCameraLocation;
		OutRotation = SmoothedCameraRotation;
		OutFOV = BaseMinFOV;
		return;
	}

	const FVelocityCameraPreset& Preset = GetPresetForMode(Mode);
	OutFOV = Preset.BaseFOV;

	// Check if vehicle mesh possesses a designated camera socket
	USceneComponent* RootComp = Pawn->GetRootComponent();
	UMeshComponent* MeshComp = Pawn->FindComponentByClass<UMeshComponent>();

	if (MeshComp && Preset.PreferredMeshSocket != NAME_None && MeshComp->DoesSocketExist(Preset.PreferredMeshSocket))
	{
		const FTransform SocketTransform = MeshComp->GetSocketTransform(Preset.PreferredMeshSocket, RTS_World);
		OutLocation = SocketTransform.GetLocation();
		OutRotation = SocketTransform.GetRotation().Rotator();
		return;
	}

	// Fallback to offset calculation
	const FVector PawnLocation = Pawn->GetActorLocation();
	const FRotator PawnRotation = Pawn->GetActorRotation();

	const float SpeedMPH = ResolveVehicleSpeedMPH();
	const float SpeedRatio = FMath::Clamp(SpeedMPH / FMath::Max(1.0f, ReferenceMaxSpeedMPH), 0.0f, 1.0f);

	FVector ScaledOffset = Preset.RelativeOffset;
	if (Preset.SpeedPullbackDistance > 0.0f)
	{
		ScaledOffset.X -= (Preset.SpeedPullbackDistance * SpeedRatio);
	}

	FRotator BaseRot = PawnRotation;
	if (Preset.bFollowYawOnly)
	{
		BaseRot = FRotator(0.0f, PawnRotation.Yaw, 0.0f);
	}

	const FVector RotatedOffset = BaseRot.RotateVector(ScaledOffset);
	OutLocation = PawnLocation + RotatedOffset;

	const FVector LookAtWorld = PawnLocation + BaseRot.RotateVector(Preset.LookAtOffset);
	const FVector LookDirection = (LookAtWorld - OutLocation).GetSafeNormal();

	OutRotation = LookDirection.Rotation();

	// Add cockpit G-force inertia
	if (Mode == EVelocityCameraMode::Cockpit)
	{
		float SteerVal = 0.0f;
		float ThrottleVal = 0.0f;
		if (TargetVelocityVehicle)
		{
			SteerVal = TargetVelocityVehicle->CurrentSteering;
			ThrottleVal = TargetVelocityVehicle->CurrentThrottle - TargetVelocityVehicle->CurrentBrake;
		}
		OutRotation.Roll += (SteerVal * 2.5f);
		OutRotation.Pitch += (ThrottleVal * 2.0f);
	}
}

FVector AVelocityCameraManager::SolveCameraCollision(const FVector& LookAtPoint, const FVector& DesiredLocation, float DeltaTime)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return DesiredLocation;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(VelocityCameraCollision), false, this);
	if (TargetPawn)
	{
		QueryParams.AddIgnoredActor(TargetPawn);
	}

	FHitResult HitResult;
	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(CollisionProbeRadius);

	const bool bHit = World->SweepSingleByChannel(
		HitResult,
		LookAtPoint,
		DesiredLocation,
		FQuat::Identity,
		CameraCollisionChannel,
		SphereShape,
		QueryParams
	);

	if (bHit && HitResult.bBlockingHit)
	{
		const FVector SafeHitLocation = HitResult.Location + (HitResult.ImpactNormal * (CollisionProbeRadius + 5.0f));
		CurrentCollisionDistanceAlpha = FMath::Clamp(HitResult.Time, 0.1f, 1.0f);
		return SafeHitLocation;
	}

	CurrentCollisionDistanceAlpha = FMath::FInterpTo(CurrentCollisionDistanceAlpha, 1.0f, DeltaTime, CollisionRecoverySpeed);
	return DesiredLocation;
}

void AVelocityCameraManager::ApplySpeedEffects(float SpeedMPH, FTViewTarget& OutVT, float DeltaTime)
{
	const float SpeedRatio = FMath::Clamp(SpeedMPH / FMath::Max(1.0f, ReferenceMaxSpeedMPH), 0.0f, 1.0f);

	// Smooth FOV widen from BaseMinFOV (e.g. 90) up to MaxSpeedFOV (e.g. 118)
	const float TargetFOV = FMath::Lerp(BaseMinFOV, MaxSpeedFOV, SpeedRatio);
	SmoothedFOV = FMath::FInterpTo(SmoothedFOV, TargetFOV, DeltaTime, FOVInterpSpeed);
	OutVT.POV.FOV = SmoothedFOV;

	// Dynamic Motion Blur
	if (bEnableMotionBlurEffect)
	{
		OutVT.POV.PostProcessSettings.bOverride_MotionBlurAmount = true;
		OutVT.POV.PostProcessSettings.MotionBlurAmount = FMath::Lerp(MinMotionBlurAmount, MaxMotionBlurAmount, SpeedRatio);

		OutVT.POV.PostProcessSettings.bOverride_MotionBlurMax = true;
		OutVT.POV.PostProcessSettings.MotionBlurMax = FMath::Lerp(1.0f, 5.0f, SpeedRatio);
	}
}

void AVelocityCameraManager::ApplyCameraShake(float SpeedMPH, float DeltaTime, FRotator& InOutRotation, FVector& InOutLocation)
{
	if (!bEnableHighSpeedShake)
	{
		return;
	}

	float ShakeIntensity = 0.0f;
	if (SpeedMPH > ShakeSpeedThresholdMPH)
	{
		const float OverSpeed = (SpeedMPH - ShakeSpeedThresholdMPH) / FMath::Max(1.0f, ReferenceMaxSpeedMPH - ShakeSpeedThresholdMPH);
		ShakeIntensity = FMath::Clamp(FMath::Square(OverSpeed), 0.0f, 1.0f);
	}

	// Blend in external wind and redline shakes
	ShakeIntensity = FMath::Max(ShakeIntensity, FMath::Max(ExternalWindShakeIntensity * 0.5f, ExternalRedlineShakeIntensity * 0.6f));

	// Decay external shake impulses
	ExternalWindShakeIntensity = FMath::FInterpTo(ExternalWindShakeIntensity, 0.0f, DeltaTime, 4.0f);
	ExternalRedlineShakeIntensity = FMath::FInterpTo(ExternalRedlineShakeIntensity, 0.0f, DeltaTime, 5.0f);

	if (ShakeIntensity <= 0.001f)
	{
		return;
	}

	ShakeTimer += DeltaTime * ShakeFrequency;

	const float PitchShake = FMath::Sin(ShakeTimer * 1.1f) * MaxShakePitchYawRoll * ShakeIntensity;
	const float YawShake   = FMath::Cos(ShakeTimer * 0.9f) * MaxShakePitchYawRoll * ShakeIntensity * 0.7f;
	const float RollShake  = FMath::Sin(ShakeTimer * 1.7f) * MaxShakePitchYawRoll * ShakeIntensity * 0.5f;
	const float ZOffsetShake = FMath::Sin(ShakeTimer * 2.3f) * (ShakeIntensity * 1.8f);

	InOutRotation.Pitch += PitchShake;
	InOutRotation.Yaw   += YawShake;
	InOutRotation.Roll  += RollShake;
	InOutLocation.Z     += ZOffsetShake;
}

void AVelocityCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	if (!TargetPawn && OutVT.Target)
	{
		SetTargetPawn(Cast<APawn>(OutVT.Target));
	}
	if (!TargetPawn && PCOwner)
	{
		SetTargetPawn(PCOwner->GetPawn());
	}

	if (!TargetPawn)
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
		return;
	}

	const FVelocityCameraPreset& Preset = GetPresetForMode(CurrentCameraMode);

	FVector IdealLocation;
	FRotator IdealRotation;
	float IdealFOV;
	ComputeModeTransform(CurrentCameraMode, TargetPawn, IdealLocation, IdealRotation, IdealFOV);

	if (bIsBlending)
	{
		BlendTimer += DeltaTime;
		BlendAlpha = FMath::Clamp(BlendTimer / FMath::Max(0.01f, BlendDuration), 0.0f, 1.0f);

		const float SmoothA = BlendAlpha * BlendAlpha * (3.0f - (2.0f * BlendAlpha));

		SmoothedCameraLocation = FMath::Lerp(BlendStartLocation, IdealLocation, SmoothA);
		SmoothedCameraRotation = FQuat::Slerp(BlendStartRotation.Quaternion(), IdealRotation.Quaternion(), SmoothA).Rotator();
		SmoothedFOV = FMath::Lerp(BlendStartFOV, IdealFOV, SmoothA);

		if (BlendAlpha >= 1.0f)
		{
			bIsBlending = false;
		}
	}
	else
	{
		SmoothedCameraLocation = FMath::VInterpTo(SmoothedCameraLocation, IdealLocation, DeltaTime, Preset.PositionLagSpeed);
		SmoothedCameraRotation = FMath::RInterpTo(SmoothedCameraRotation, IdealRotation, DeltaTime, Preset.RotationLagSpeed);
	}

	FVector FinalLocation = SmoothedCameraLocation;
	if (Preset.bEnableCollision)
	{
		const FVector LookAtWorld = TargetPawn->GetActorLocation() + TargetPawn->GetActorRotation().RotateVector(Preset.LookAtOffset);
		FinalLocation = SolveCameraCollision(LookAtWorld, SmoothedCameraLocation, DeltaTime);
	}

	FRotator FinalRotation = SmoothedCameraRotation;

	const float SpeedMPH = ResolveVehicleSpeedMPH();
	ApplyCameraShake(SpeedMPH, DeltaTime, FinalRotation, FinalLocation);

	OutVT.POV.Location = FinalLocation;
	OutVT.POV.Rotation = FinalRotation;

	ApplySpeedEffects(SpeedMPH, OutVT, DeltaTime);
}
