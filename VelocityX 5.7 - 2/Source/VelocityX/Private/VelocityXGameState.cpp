// Copyright VELOCITY X Racing. All Rights Reserved.

#include "VelocityXGameState.h"

AVelocityXGameState::AVelocityXGameState()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentTrackTime = 0.0f;
	CurrentLapTime = 0.0f;
	BestLapTime = 0.0f;
	bTimerRunning = false;

	CurrentLap = 1;
	TotalLaps = 3;
	bRaceFinished = false;

	// Populate initial default leaderboard records
	FVelocityXHighScore DefaultRecord1;
	DefaultRecord1.PlayerName = TEXT("Phantom");
	DefaultRecord1.RaceTimeSeconds = 112.45f;
	DefaultRecord1.BestLapTimeSeconds = 36.12f;
	DefaultRecord1.TopSpeedKPH = 348.5f;
	DefaultRecord1.CarName = TEXT("Hypercar");
	HighScores.Add(DefaultRecord1);

	FVelocityXHighScore DefaultRecord2;
	DefaultRecord2.PlayerName = TEXT("Apex");
	DefaultRecord2.RaceTimeSeconds = 118.80f;
	DefaultRecord2.BestLapTimeSeconds = 38.25f;
	DefaultRecord2.TopSpeedKPH = 325.0f;
	DefaultRecord2.CarName = TEXT("TrackSpec");
	HighScores.Add(DefaultRecord2);

	FVelocityXHighScore DefaultRecord3;
	DefaultRecord3.PlayerName = TEXT("Vortex");
	DefaultRecord3.RaceTimeSeconds = 124.15f;
	DefaultRecord3.BestLapTimeSeconds = 40.50f;
	DefaultRecord3.TopSpeedKPH = 308.2f;
	DefaultRecord3.CarName = TEXT("Supercar");
	HighScores.Add(DefaultRecord3);
}

void AVelocityXGameState::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("AVelocityXGameState initialized."));
}

void AVelocityXGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bTimerRunning && !bRaceFinished)
	{
		CurrentTrackTime += DeltaTime;
		CurrentLapTime += DeltaTime;

		OnRaceTimeUpdated.Broadcast(CurrentTrackTime);
	}
}

void AVelocityXGameState::StartTimer()
{
	bTimerRunning = true;
	bRaceFinished = false;
}

void AVelocityXGameState::PauseTimer()
{
	bTimerRunning = false;
}

void AVelocityXGameState::ResumeTimer()
{
	if (!bRaceFinished)
	{
		bTimerRunning = true;
	}
}

void AVelocityXGameState::ResetTimer()
{
	CurrentTrackTime = 0.0f;
	CurrentLapTime = 0.0f;
	bTimerRunning = false;
	bRaceFinished = false;
	CurrentLap = 1;
}

FString AVelocityXGameState::GetFormattedTrackTime() const
{
	return FormatTimeToString(CurrentTrackTime);
}

FString AVelocityXGameState::GetFormattedBestLapTime() const
{
	if (BestLapTime <= 0.0f)
	{
		return TEXT("--:--.---");
	}
	return FormatTimeToString(BestLapTime);
}

void AVelocityXGameState::SetTotalLaps(int32 InTotalLaps)
{
	TotalLaps = FMath::Max(1, InTotalLaps);
}

void AVelocityXGameState::CompleteLap(float LapTime)
{
	float RecordedLapTime = (LapTime > 0.0f) ? LapTime : CurrentLapTime;

	if (BestLapTime <= 0.0f || RecordedLapTime < BestLapTime)
	{
		BestLapTime = RecordedLapTime;
		UE_LOG(LogTemp, Log, TEXT("New Best Lap Time: %s"), *FormatTimeToString(BestLapTime));
	}

	CurrentLapTime = 0.0f;

	if (CurrentLap >= TotalLaps)
	{
		bRaceFinished = true;
		bTimerRunning = false;
		UE_LOG(LogTemp, Display, TEXT("Race Finished! Final Track Time: %s"), *FormatTimeToString(CurrentTrackTime));
	}
	else
	{
		CurrentLap++;
	}

	OnLapCompleted.Broadcast(CurrentLap);
}

void AVelocityXGameState::AddHighScore(const FVelocityXHighScore& Record)
{
	HighScores.Add(Record);

	// Sort high scores ascending by race time
	HighScores.Sort([](const FVelocityXHighScore& A, const FVelocityXHighScore& B)
	{
		return A.RaceTimeSeconds < B.RaceTimeSeconds;
	});

	// Keep top 10 records
	if (HighScores.Num() > 10)
	{
		HighScores.SetNum(10);
	}

	OnNewHighScore.Broadcast(Record);
}

void AVelocityXGameState::ClearHighScores()
{
	HighScores.Empty();
}

bool AVelocityXGameState::IsNewHighScore(float TimeSeconds) const
{
	if (HighScores.Num() < 10)
	{
		return true;
	}
	return TimeSeconds < HighScores.Last().RaceTimeSeconds;
}

FString AVelocityXGameState::FormatTimeToString(float TimeInSeconds)
{
	if (TimeInSeconds < 0.0f)
	{
		TimeInSeconds = 0.0f;
	}

	const int32 Minutes = FMath::FloorToInt(TimeInSeconds / 60.0f);
	const float SecondsRemaining = FMath::Fmod(TimeInSeconds, 60.0f);
	const int32 Seconds = FMath::FloorToInt(SecondsRemaining);
	const int32 Milliseconds = FMath::FloorToInt((SecondsRemaining - Seconds) * 1000.0f);

	return FString::Printf(TEXT("%02d:%02d.%03d"), Minutes, Seconds, Milliseconds);
}
