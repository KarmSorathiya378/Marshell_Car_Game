// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VELOCITYXGameMode.generated.h"

// Forward declarations
class ABaseCar;
class AVelocityPlayerController;

/**
 * Car types available in VELOCITY X
 */
UENUM(BlueprintType)
enum class ECarType : uint8
{
	Supercar      UMETA(DisplayName = "Supercar"),
	TrackSupercar UMETA(DisplayName = "Track Supercar"),
	Hypercar      UMETA(DisplayName = "Hypercar")
};

/**
 * Game state machine enumeration (Menu -> Race -> Pause -> Resume -> End)
 */
UENUM(BlueprintType)
enum class EGameState : uint8
{
	Menu       UMETA(DisplayName = "Menu"),
	Race       UMETA(DisplayName = "Race"),
	Pause      UMETA(DisplayName = "Pause"),
	Resume     UMETA(DisplayName = "Resume"),
	End        UMETA(DisplayName = "End"),

	// Aliases for backward compatibility
	MainMenu   UMETA(DisplayName = "Main Menu"),
	CarSelect  UMETA(DisplayName = "Car Select"),
	Racing     UMETA(DisplayName = "Racing"),
	Paused     UMETA(DisplayName = "Paused"),
	GameOver   UMETA(DisplayName = "Game Over")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameStateTransitionedSignature, EGameState, PreviousState, EGameState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChangedSignature, EGameState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCarSelectedSignature, ECarType, SelectedType);

/**
 * VELOCITY X Main Game Mode
 * Features a full state machine (Menu, Race, Pause, Resume, End), car selection, and timing logic.
 */
UCLASS(Blueprintable, BlueprintType)
class VELOCITYX_API AVELOCITYXGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AVELOCITYXGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// --- State Machine ---
	UFUNCTION(BlueprintCallable, Category = "Game State Machine")
	bool SetGameState(EGameState NewState);

	UFUNCTION(BlueprintCallable, Category = "Game State Machine")
	bool CanTransitionTo(EGameState TargetState) const;

	UFUNCTION(BlueprintPure, Category = "Game State Machine")
	EGameState GetGameState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Game State Machine")
	EGameState GetPreviousState() const { return PreviousState; }

	UFUNCTION(BlueprintPure, Category = "Game State Machine")
	FString GetGameStateAsString(EGameState State) const;

	// Dedicated transition commands
	UFUNCTION(BlueprintCallable, Category = "Game State Machine")
	void TransitionToMenu();

	UFUNCTION(BlueprintCallable, Category = "Game State Machine")
	void TransitionToRace();

	UFUNCTION(BlueprintCallable, Category = "Game State Machine")
	void TransitionToPause();

	UFUNCTION(BlueprintCallable, Category = "Game State Machine")
	void TransitionToResume();

	UFUNCTION(BlueprintCallable, Category = "Game State Machine")
	void TransitionToEnd();

	// --- Car Selection ---
	UFUNCTION(BlueprintCallable, Category = "Car Selection")
	void SelectCar(ECarType CarType);

	UFUNCTION(BlueprintPure, Category = "Car Selection")
	ECarType GetSelectedCarType() const { return SelectedCarType; }

	// --- Timer / Race Logic ---
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StartTimer();

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void PauseTimer();

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void ResetTimer();

	UFUNCTION(BlueprintPure, Category = "Timer")
	float GetElapsedTime() const { return ElapsedTime; }

	// --- Events / Delegates ---
	UPROPERTY(BlueprintAssignable, Category = "Game State Machine")
	FOnGameStateTransitionedSignature OnStateTransitioned;

	UPROPERTY(BlueprintAssignable, Category = "Game State Machine")
	FOnGameStateChangedSignature OnGameStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Car Selection")
	FOnCarSelectedSignature OnCarSelected;

	// Blueprint Implementable State Notification Hooks
	UFUNCTION(BlueprintImplementableEvent, Category = "Game State Machine")
	void OnEnterMenu();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game State Machine")
	void OnEnterRace();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game State Machine")
	void OnEnterPause();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game State Machine")
	void OnEnterResume();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game State Machine")
	void OnEnterEnd();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnStateChangeUI(EGameState NewState);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game State")
	EGameState CurrentState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	EGameState PreviousState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Selection")
	ECarType SelectedCarType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer")
	float ElapsedTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer")
	bool bTimerRunning;

	virtual void HandleStateEnter(EGameState NewState);
	virtual void HandleStateExit(EGameState OldState);

	UFUNCTION()
	void UpdateTimer(float DeltaTime);
};
