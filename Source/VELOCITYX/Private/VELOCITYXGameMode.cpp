// Copyright VELOCITY X Racing. All Rights Reserved.

#include "VELOCITYXGameMode.h"
#include "VelocityPlayerController.h"
#include "TimerManager.h"

AVELOCITYXGameMode::AVELOCITYXGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.016f; // ~60 FPS update frequency

	CurrentState = EGameState::Menu;
	PreviousState = EGameState::Menu;
	SelectedCarType = ECarType::Supercar;
	ElapsedTime = 0.0f;
	bTimerRunning = false;

	PlayerControllerClass = AVelocityPlayerController::StaticClass();
}

void AVELOCITYXGameMode::BeginPlay()
{
	Super::BeginPlay();

	HandleStateEnter(CurrentState);
	OnStateChangeUI(CurrentState);
	OnGameStateChanged.Broadcast(CurrentState);
	OnStateTransitioned.Broadcast(PreviousState, CurrentState);
}

void AVELOCITYXGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTimer(DeltaTime);
}

bool AVELOCITYXGameMode::CanTransitionTo(EGameState TargetState) const
{
	if (CurrentState == TargetState)
	{
		return false;
	}

	// State machine transition validation rules
	switch (TargetState)
	{
	case EGameState::Menu:
	case EGameState::MainMenu:
		return true; // Can return to menu from any state

	case EGameState::Race:
	case EGameState::Racing:
		return CurrentState == EGameState::Menu || 
		       CurrentState == EGameState::MainMenu || 
		       CurrentState == EGameState::CarSelect || 
		       CurrentState == EGameState::Resume || 
		       CurrentState == EGameState::End || 
		       CurrentState == EGameState::GameOver;

	case EGameState::Pause:
	case EGameState::Paused:
		return CurrentState == EGameState::Race || 
		       CurrentState == EGameState::Racing;

	case EGameState::Resume:
		return CurrentState == EGameState::Pause || 
		       CurrentState == EGameState::Paused;

	case EGameState::End:
	case EGameState::GameOver:
		return CurrentState == EGameState::Race || 
		       CurrentState == EGameState::Racing || 
		       CurrentState == EGameState::Pause || 
		       CurrentState == EGameState::Paused;

	case EGameState::CarSelect:
		return CurrentState == EGameState::Menu || 
		       CurrentState == EGameState::MainMenu;

	default:
		return true;
	}
}

bool AVELOCITYXGameMode::SetGameState(EGameState NewState)
{
	if (CurrentState == NewState)
	{
		return false;
	}

	if (!CanTransitionTo(NewState))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot transition state machine from %s to %s"), 
			*GetGameStateAsString(CurrentState), *GetGameStateAsString(NewState));
		return false;
	}

	EGameState OldState = CurrentState;
	PreviousState = OldState;
	CurrentState = NewState;

	HandleStateExit(OldState);
	HandleStateEnter(NewState);

	OnStateChangeUI(CurrentState);
	OnGameStateChanged.Broadcast(CurrentState);
	OnStateTransitioned.Broadcast(PreviousState, CurrentState);

	UE_LOG(LogTemp, Display, TEXT("State Machine: %s -> %s"), 
		*GetGameStateAsString(PreviousState), *GetGameStateAsString(CurrentState));

	// If transitioning to Resume, seamlessly transition into Race state
	if (CurrentState == EGameState::Resume)
	{
		SetGameState(EGameState::Race);
	}

	return true;
}

void AVELOCITYXGameMode::TransitionToMenu()
{
	SetGameState(EGameState::Menu);
}

void AVELOCITYXGameMode::TransitionToRace()
{
	SetGameState(EGameState::Race);
}

void AVELOCITYXGameMode::TransitionToPause()
{
	SetGameState(EGameState::Pause);
}

void AVELOCITYXGameMode::TransitionToResume()
{
	SetGameState(EGameState::Resume);
}

void AVELOCITYXGameMode::TransitionToEnd()
{
	SetGameState(EGameState::End);
}

void AVELOCITYXGameMode::HandleStateEnter(EGameState NewState)
{
	switch (NewState)
	{
	case EGameState::Menu:
	case EGameState::MainMenu:
		PauseTimer();
		OnEnterMenu();
		break;

	case EGameState::Race:
	case EGameState::Racing:
		StartTimer();
		OnEnterRace();
		break;

	case EGameState::Pause:
	case EGameState::Paused:
		PauseTimer();
		OnEnterPause();
		break;

	case EGameState::Resume:
		StartTimer();
		OnEnterResume();
		break;

	case EGameState::End:
	case EGameState::GameOver:
		PauseTimer();
		OnEnterEnd();
		break;

	default:
		break;
	}
}

void AVELOCITYXGameMode::HandleStateExit(EGameState OldState)
{
	// Exit state cleanup if required
}

FString AVELOCITYXGameMode::GetGameStateAsString(EGameState State) const
{
	switch (State)
	{
	case EGameState::Menu:      return TEXT("Menu");
	case EGameState::Race:      return TEXT("Race");
	case EGameState::Pause:     return TEXT("Pause");
	case EGameState::Resume:    return TEXT("Resume");
	case EGameState::End:       return TEXT("End");
	case EGameState::MainMenu:  return TEXT("MainMenu");
	case EGameState::CarSelect: return TEXT("CarSelect");
	case EGameState::Racing:    return TEXT("Racing");
	case EGameState::Paused:    return TEXT("Paused");
	case EGameState::GameOver:  return TEXT("GameOver");
	default:                    return TEXT("Unknown");
	}
}

void AVELOCITYXGameMode::SelectCar(ECarType CarType)
{
	SelectedCarType = CarType;
	OnCarSelected.Broadcast(SelectedCarType);
	UE_LOG(LogTemp, Display, TEXT("Selected car type: %d"), static_cast<int32>(CarType));
}

void AVELOCITYXGameMode::StartTimer()
{
	bTimerRunning = true;
}

void AVELOCITYXGameMode::PauseTimer()
{
	bTimerRunning = false;
}

void AVELOCITYXGameMode::ResetTimer()
{
	ElapsedTime = 0.0f;
	bTimerRunning = false;
}

void AVELOCITYXGameMode::UpdateTimer(float DeltaTime)
{
	if (bTimerRunning && (CurrentState == EGameState::Race || CurrentState == EGameState::Racing))
	{
		ElapsedTime += DeltaTime;
	}
}
