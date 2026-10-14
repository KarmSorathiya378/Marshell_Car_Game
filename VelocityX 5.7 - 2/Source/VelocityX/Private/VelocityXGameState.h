// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "VelocityXGameState.generated.h"

/** High score record entry */
USTRUCT(BlueprintType)
struct VELOCITYX_API FVelocityXHighScore
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "High Score")
	FString PlayerName = TEXT("Driver");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "High Score")
	float RaceTimeSeconds = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "High Score")
	float BestLapTimeSeconds = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "High Score")
	float TopSpeedKPH = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "High Score")
	FString CarName = TEXT("Supercar");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "High Score")
	FDateTime Timestamp = FDateTime::UtcNow();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLapCompleted, int32, NewLap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRaceTimeUpdated, float, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewHighScore, const FVelocityXHighScore&, NewRecord);

/**
 * AVelocityXGameState
 * Tracks live race timing, split times, lap counters, and persistent high scores.
 */
UCLASS()
class VELOCITYX_API AVelocityXGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AVelocityXGameState();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// --- Race Timing ---
	UFUNCTION(BlueprintCallable, Category = "VelocityX|Timing")
	void StartTimer();

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Timing")
	void PauseTimer();

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Timing")
	void ResumeTimer();

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Timing")
	void ResetTimer();

	UFUNCTION(BlueprintPure, Category = "VelocityX|Timing")
	float GetTrackTime() const { return CurrentTrackTime; }

	UFUNCTION(BlueprintPure, Category = "VelocityX|Timing")
	FString GetFormattedTrackTime() const;

	UFUNCTION(BlueprintPure, Category = "VelocityX|Timing")
	float GetBestLapTime() const { return BestLapTime; }

	UFUNCTION(BlueprintPure, Category = "VelocityX|Timing")
	FString GetFormattedBestLapTime() const;

	UFUNCTION(BlueprintPure, Category = "VelocityX|Timing")
	bool IsTimerRunning() const { return bTimerRunning; }

	// --- Lap Counter & Progress ---
	UFUNCTION(BlueprintCallable, Category = "VelocityX|Laps")
	void SetTotalLaps(int32 InTotalLaps);

	UFUNCTION(BlueprintPure, Category = "VelocityX|Laps")
	int32 GetCurrentLap() const { return CurrentLap; }

	UFUNCTION(BlueprintPure, Category = "VelocityX|Laps")
	int32 GetTotalLaps() const { return TotalLaps; }

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Laps")
	void CompleteLap(float LapTime);

	UFUNCTION(BlueprintPure, Category = "VelocityX|Laps")
	bool IsFinalLap() const { return CurrentLap >= TotalLaps; }

	UFUNCTION(BlueprintPure, Category = "VelocityX|Laps")
	bool HasFinishedRace() const { return bRaceFinished; }

	// --- High Scores & Records ---
	UFUNCTION(BlueprintCallable, Category = "VelocityX|HighScores")
	void AddHighScore(const FVelocityXHighScore& Record);

	UFUNCTION(BlueprintPure, Category = "VelocityX|HighScores")
	TArray<FVelocityXHighScore> GetHighScores() const { return HighScores; }

	UFUNCTION(BlueprintCallable, Category = "VelocityX|HighScores")
	void ClearHighScores();

	UFUNCTION(BlueprintPure, Category = "VelocityX|HighScores")
	bool IsNewHighScore(float TimeSeconds) const;

	// --- Delegates ---
	UPROPERTY(BlueprintAssignable, Category = "VelocityX|Events")
	FOnLapCompleted OnLapCompleted;

	UPROPERTY(BlueprintAssignable, Category = "VelocityX|Events")
	FOnRaceTimeUpdated OnRaceTimeUpdated;

	UPROPERTY(BlueprintAssignable, Category = "VelocityX|Events")
	FOnNewHighScore OnNewHighScore;

protected:
	// Live timing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VelocityX|Timing")
	float CurrentTrackTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VelocityX|Timing")
	float CurrentLapTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VelocityX|Timing")
	float BestLapTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VelocityX|Timing")
	bool bTimerRunning;

	// Laps
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|Laps")
	int32 CurrentLap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|Laps")
	int32 TotalLaps;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VelocityX|Laps")
	bool bRaceFinished;

	// High scores list (sorted ascending by time)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VelocityX|HighScores")
	TArray<FVelocityXHighScore> HighScores;

	static FString FormatTimeToString(float TimeInSeconds);
};
