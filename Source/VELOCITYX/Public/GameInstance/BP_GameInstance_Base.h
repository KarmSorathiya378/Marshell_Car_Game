// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VELOCITYXGameInstance.h"
#include "BP_GameInstance_Base.generated.h"

/**
 * Blueprint Game Instance Base Class
 * Direct C++ parent for BP_GameInstance in Content/Blueprints/.
 * Provides dedicated Blueprint Implementable Events and Blueprint Callable helpers
 * for scoring, leaderboard ranks, and telemetry visualization.
 */
UCLASS(Blueprintable, BlueprintType)
class VELOCITYX_API UBP_GameInstance_Base : public UVELOCITYXGameInstance
{
	GENERATED_BODY()

public:
	UBP_GameInstance_Base();

	// --- Blueprint Scoring Hooks ---
	UFUNCTION(BlueprintImplementableEvent, Category = "High Score|Hooks")
	void BP_OnNewHighScoreAchieved(const FHighScoreEntry& HighScoreEntry, int32 Rank);

	UFUNCTION(BlueprintImplementableEvent, Category = "Telemetry|Hooks")
	void BP_OnTelemetryUpdated(const FTelemetrySnapshot& Snapshot);

	UFUNCTION(BlueprintImplementableEvent, Category = "Telemetry|Hooks")
	void BP_OnNewBestLapTime(float LapTime, int32 LapNumber);

	// --- Score Helper Functions ---
	UFUNCTION(BlueprintCallable, Category = "High Score")
	void AddDriftScore(float DriftDuration, float DriftAngleMultiplier);

	UFUNCTION(BlueprintCallable, Category = "High Score")
	void AddSpeedScore(float SpeedMPH, float DurationSeconds);

	UFUNCTION(BlueprintCallable, Category = "High Score")
	void AddCleanLapBonus(int32 LapNumber);

protected:
	virtual void Init() override;
};
