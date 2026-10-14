// Copyright VELOCITY X Racing. All Rights Reserved.

#include "GameInstance/BP_GameInstance_Base.h"

UBP_GameInstance_Base::UBP_GameInstance_Base()
{
}

void UBP_GameInstance_Base::Init()
{
	Super::Init();

	// Bind internal telemetry delegates to Blueprint hooks
	if (TelemetryTracker)
	{
		TelemetryTracker->OnTelemetryUpdated.AddDynamic(this, &UBP_GameInstance_Base::BP_OnTelemetryUpdated);
		TelemetryTracker->OnNewBestLap.AddDynamic(this, &UBP_GameInstance_Base::BP_OnNewBestLapTime);
	}

	OnNewHighScore.AddDynamic(this, &UBP_GameInstance_Base::BP_OnNewHighScoreAchieved);
}

void UBP_GameInstance_Base::AddDriftScore(float DriftDuration, float DriftAngleMultiplier)
{
	if (DriftDuration <= 0.0f)
	{
		return;
	}

	int64 Points = static_cast<int64>(DriftDuration * 100.0f * FMath::Max(1.0f, DriftAngleMultiplier));
	AddScore(Points);
	UE_LOG(LogTemp, Display, TEXT("Drift Score Awarded: +%lld pts"), Points);
}

void UBP_GameInstance_Base::AddSpeedScore(float SpeedMPH, float DurationSeconds)
{
	if (SpeedMPH < 100.0f || DurationSeconds <= 0.0f)
	{
		return;
	}

	// High speed score multiplier for driving over 100 MPH
	float SpeedExcess = SpeedMPH - 100.0f;
	int64 Points = static_cast<int64>(SpeedExcess * DurationSeconds * 10.0f);
	AddScore(Points);
}

void UBP_GameInstance_Base::AddCleanLapBonus(int32 LapNumber)
{
	int64 BonusPoints = 5000 + (LapNumber * 1000);
	AddScore(BonusPoints);
	UE_LOG(LogTemp, Display, TEXT("Clean Lap Bonus Awarded for Lap %d: +%lld pts"), LapNumber, BonusPoints);
}
