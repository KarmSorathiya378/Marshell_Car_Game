// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VelocityXGameState.h"
#include "VelocityXGameModeBase.generated.h"

class AVelocityXPlayerController;

/** Available car models in VELOCITY X */
UENUM(BlueprintType)
enum class EVelocityXCarType : uint8
{
	Supercar    UMETA(DisplayName = "Supercar (Balanced)"),
	TrackSpec   UMETA(DisplayName = "TrackSpec (High Downforce)"),
	Hypercar    UMETA(DisplayName = "Hypercar (Extreme Speed)")
};

/** Match / race progression states */
UENUM(BlueprintType)
enum class EVelocityXMatchState : uint8
{
	CarSelection UMETA(DisplayName = "Car Selection"),
	Countdown    UMETA(DisplayName = "Countdown"),
	Racing       UMETA(DisplayName = "Racing"),
	Paused       UMETA(DisplayName = "Paused"),
	Finished     UMETA(DisplayName = "Finished")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVelocityMatchStateChanged, EVelocityXMatchState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCarSelected, EVelocityXCarType, SelectedCar);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePauseChanged, bool, bIsPaused);

/**
 * AVelocityXGameModeBase
 * Master Game Mode for VELOCITY X:
 * - Match state management (CarSelection, Countdown, Racing, Paused, Finished)
 * - Vehicle selection and dynamic pawn spawning/possession
 * - Central pause / unpause routing and UI context switching
 * - High-precision session racing timer integrated with AVelocityXGameState
 */
UCLASS()
class VELOCITYX_API AVelocityXGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AVelocityXGameModeBase();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// --- Match State Management ---
	UFUNCTION(BlueprintCallable, Category = "VelocityX|GameMode")
	void SetMatchState(EVelocityXMatchState NewState);

	UFUNCTION(BlueprintPure, Category = "VelocityX|GameMode")
	EVelocityXMatchState GetMatchState() const { return CurrentMatchState; }

	UFUNCTION(BlueprintPure, Category = "VelocityX|GameMode")
	bool IsRacing() const { return CurrentMatchState == EVelocityXMatchState::Racing; }

	// --- Car Selection State ---
	UFUNCTION(BlueprintCallable, Category = "VelocityX|CarSelection")
	void SelectCar(EVelocityXCarType InCarType);

	UFUNCTION(BlueprintPure, Category = "VelocityX|CarSelection")
	EVelocityXCarType GetSelectedCarType() const { return SelectedCarType; }

	UFUNCTION(BlueprintCallable, Category = "VelocityX|CarSelection")
	void SpawnAndPossessSelectedVehicle(APlayerController* PlayerController);

	// --- Pause / Unpause Logic ---
	UFUNCTION(BlueprintCallable, Category = "VelocityX|Pause")
	bool TogglePauseGame();

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Pause")
	void PauseGame();

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Pause")
	void UnpauseGame();

	UFUNCTION(BlueprintPure, Category = "VelocityX|Pause")
	bool IsGamePaused() const { return bIsGamePaused; }

	// --- Session Timer for Races ---
	UFUNCTION(BlueprintCallable, Category = "VelocityX|Timer")
	void StartRace();

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Timer")
	void EndRace();

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Timer")
	void ResetSessionTimer();

	UFUNCTION(BlueprintPure, Category = "VelocityX|Timer")
	float GetSessionTime() const;

	UFUNCTION(BlueprintPure, Category = "VelocityX|Timer")
	FString GetFormattedSessionTime() const;

	// --- Vehicle Classes for Selection ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|Vehicles")
	TSubclassOf<APawn> SupercarClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|Vehicles")
	TSubclassOf<APawn> TrackSpecClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|Vehicles")
	TSubclassOf<APawn> HypercarClass;

	// --- Delegates ---
	UPROPERTY(BlueprintAssignable, Category = "VelocityX|Events")
	FOnVelocityMatchStateChanged OnMatchStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "VelocityX|Events")
	FOnCarSelected OnCarSelected;

	UPROPERTY(BlueprintAssignable, Category = "VelocityX|Events")
	FOnGamePauseChanged OnGamePauseChanged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VelocityX|State")
	EVelocityXMatchState CurrentMatchState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VelocityX|State")
	EVelocityXMatchState PrePauseState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VelocityX|CarSelection")
	EVelocityXCarType SelectedCarType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VelocityX|Pause")
	bool bIsGamePaused;

	// Cached GameState helper
	AVelocityXGameState* GetVelocityGameState() const;
};
