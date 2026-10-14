// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "VelocityCameraShake.generated.h"

/**
 * Camera shake for high speed turbulence and engine rumble in VELOCITY X
 */
UCLASS()
class VELOCITYX_API UVelocityHighSpeedCameraShake : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	UVelocityHighSpeedCameraShake(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
	float OscillationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
	float OscillationBlendInTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
	float OscillationBlendOutTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
	FRotator RotationalAmplitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
	FRotator RotationalFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
	FVector LocationalAmplitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
	FVector LocationalFrequency;
};

/**
 * Camera shake for hard impacts, crashes, and sudden deceleration in VELOCITY X
 */
UCLASS()
class VELOCITYX_API UVelocityImpactCameraShake : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	UVelocityImpactCameraShake(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
	float ImpactIntensity;
};
