// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Telemetry/VelocityTelemetryTracker.h"
#include "VelocityGameInstance.generated.h"

/**
 * High score record entry
 */
USTRUCT(BlueprintType)
struct FHighScoreEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "High Score")
	FString PlayerName = TEXT("Driver");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "High Score")
	int64 Score = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "High Score")
	float BestLapTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "High Score")
	FString CarUsed = TEXT("Supercar");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "High Score")
	FString Timestamp = TEXT("");
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreChangedSignature, int64, NewScore, int64, DeltaPoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewHighScoreSignature, const FHighScoreEntry&, NewEntry, int32, Rank);

/**
 * Velocity Game Instance
 * C++ base for BP_GameInstance.
 * Provides high score persistence, telemetry tracking, lap timing, and blueprint-accessible metrics.
 */
UCLASS(Blueprintable, BlueprintType)
class VELOCITYX_API UVelocityGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UVelocityGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

	// =========================================================================
	// TELEMETRY TRACKING (Exposed to Blueprints & UI)
	// =========================================================================
	UFUNCTION(BlueprintPure, Category = "Telemetry")
	UVelocityTelemetryTracker* GetTelemetryTracker() const { return TelemetryTracker; }

	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void UpdateTelemetry(float InSpeedMPH, float InSpeedKPH, float InRPM, int32 InGear, float DeltaTime);

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	FTelemetrySnapshot GetTelemetrySnapshot() const;

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	float GetCurrentSpeedMPH() const;

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	float GetCurrentSpeedKPH() const;

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	float GetCurrentRPM() const;

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	int32 GetCurrentGear() const;

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	float GetCurrentLapTime() const;

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	float GetBestLapTime() const;

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	float GetLastLapTime() const;

	UFUNCTION(BlueprintPure, Category = "Telemetry")
	int32 GetCurrentLapNumber() const;

	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void StartNewLap();

	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void CompleteCurrentLap();

	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void RecordLapTime(float LapTime);

	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void ResetTelemetry();

	// =========================================================================
	// SCORE & HIGH SCORE SYSTEM
	// =========================================================================
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int64 Points);

	UFUNCTION(BlueprintPure, Category = "Score")
	int64 GetCurrentScore() const { return CurrentScore; }

	UFUNCTION(BlueprintCallable, Category = "Score")
	void ResetScore();

	UFUNCTION(BlueprintCallable, Category = "Score")
	bool SubmitHighScore(const FString& InPlayerName, const FString& InCarUsed, float InBestLap);

	UFUNCTION(BlueprintPure, Category = "Score")
	bool IsHighScore(int64 InScore) const;

	UFUNCTION(BlueprintPure, Category = "Score")
	TArray<FHighScoreEntry> GetHighScores() const { return HighScores; }

	UFUNCTION(BlueprintPure, Category = "Score")
	int64 GetTopScore() const;

	UFUNCTION(BlueprintCallable, Category = "Score")
	void ClearHighScores();

	UFUNCTION(BlueprintCallable, Category = "Score")
	bool SaveHighScoresToDisk();

	UFUNCTION(BlueprintCallable, Category = "Score")
	bool LoadHighScoresFromDisk();

	// =========================================================================
	// EVENTS / DELEGATES
	// =========================================================================
	UPROPERTY(BlueprintAssignable, Category = "Score|Events")
	FOnScoreChangedSignature OnScoreChanged;

	UPROPERTY(BlueprintAssignable, Category = "Score|Events")
	FOnNewHighScoreSignature OnNewHighScore;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Telemetry")
	UVelocityTelemetryTracker* TelemetryTracker;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	int64 CurrentScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
	int32 MaxSavedHighScores;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	TArray<FHighScoreEntry> HighScores;

	FString GetHighScoreFilePath() const;
};
