// Copyright VELOCITY X Racing. All Rights Reserved.

#include "VelocityCameraShake.h"

UVelocityHighSpeedCameraShake::UVelocityHighSpeedCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OscillationDuration = 0.5f;
	OscillationBlendInTime = 0.1f;
	OscillationBlendOutTime = 0.1f;

	RotationalAmplitude = FRotator(0.4f, 0.4f, 0.6f);
	RotationalFrequency = FRotator(18.0f, 15.0f, 22.0f);

	LocationalAmplitude = FVector(0.5f, 0.5f, 1.2f);
	LocationalFrequency = FVector(20.0f, 20.0f, 30.0f);
}

UVelocityImpactCameraShake::UVelocityImpactCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ImpactIntensity = 2.5f;
}
