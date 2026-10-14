// Copyright VELOCITY X Racing. All Rights Reserved.

#include "VelocityXGameModeBase.h"
#include "VelocityXPlayerController.h"
#include "VelocityXGameState.h"
#include "VelocityXCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

AVelocityXGameModeBase::AVelocityXGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Assign core default classes
	PlayerControllerClass = AVelocityXPlayerController::StaticClass();
	GameStateClass = AVelocityXGameState::StaticClass();
	DefaultPawnClass = AVelocityXCharacter::StaticClass();

	// Defaults
	CurrentMatchState = EVelocityXMatchState::CarSelection;
	PrePauseState = EVelocityXMatchState::CarSelection;
	SelectedCarType = EVelocityXCarType::Supercar;
	bIsGamePaused = false;
}

void AVelocityXGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Display, TEXT("AVelocityXGameModeBase initialized in state: CarSelection"));
}

void AVelocityXGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVelocityXGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AVelocityXPlayerController* VelocityPC = Cast<AVelocityXPlayerController>(NewPlayer);
	if (VelocityPC)
	{
		UE_LOG(LogTemp, Log, TEXT("Player logged in with AVelocityXPlayerController."));
	}
}

void AVelocityXGameModeBase::SetMatchState(EVelocityXMatchState NewState)
{
	if (CurrentMatchState == NewState)
	{
		return;
	}

	const EVelocityXMatchState OldState = CurrentMatchState;
	CurrentMatchState = NewState;

	UE_LOG(LogTemp, Display, TEXT("Match State Changed: %d -> %d"), static_cast<int32>(OldState), static_cast<int32>(NewState));

	// Synchronize timer with state
	AVelocityXGameState* GS = GetVelocityGameState();
	if (GS)
	{
		if (NewState == EVelocityXMatchState::Racing)
		{
			GS->StartTimer();
		}
		else if (NewState == EVelocityXMatchState::Paused)
		{
			GS->PauseTimer();
		}
		else if (NewState == EVelocityXMatchState::Finished)
		{
			GS->PauseTimer();
		}
	}

	OnMatchStateChanged.Broadcast(NewState);
}

void AVelocityXGameModeBase::SelectCar(EVelocityXCarType InCarType)
{
	SelectedCarType = InCarType;
	UE_LOG(LogTemp, Display, TEXT("Car selected: %d"), static_cast<int32>(InCarType));

	OnCarSelected.Broadcast(InCarType);
}

void AVelocityXGameModeBase::SpawnAndPossessSelectedVehicle(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}

	TSubclassOf<APawn> ChosenClass = nullptr;
	switch (SelectedCarType)
	{
	case EVelocityXCarType::Supercar:
		ChosenClass = SupercarClass ? SupercarClass : DefaultPawnClass;
		break;
	case EVelocityXCarType::TrackSpec:
		ChosenClass = TrackSpecClass ? TrackSpecClass : DefaultPawnClass;
		break;
	case EVelocityXCarType::Hypercar:
		ChosenClass = HypercarClass ? HypercarClass : DefaultPawnClass;
		break;
	default:
		ChosenClass = DefaultPawnClass;
		break;
	}

	// Find suitable spawn location from PlayerStart
	FTransform SpawnTransform = FTransform::Identity;
	AActor* BestStart = FindPlayerStart(PlayerController);
	if (BestStart)
	{
		SpawnTransform = BestStart->GetActorTransform();
	}
	else
	{
		APawn* CurrentPawn = PlayerController->GetPawn();
		if (CurrentPawn)
		{
			SpawnTransform = CurrentPawn->GetActorTransform();
		}
	}

	// Elevate slightly to avoid clipping through ground geometry
	FVector SpawnLocation = SpawnTransform.GetLocation() + FVector(0.0f, 0.0f, 35.0f);
	SpawnTransform.SetLocation(SpawnLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APawn* OldPawn = PlayerController->GetPawn();
	APawn* NewVehicle = GetWorld()->SpawnActor<APawn>(ChosenClass, SpawnTransform, SpawnParams);

	if (NewVehicle)
	{
		PlayerController->Possess(NewVehicle);

		if (OldPawn && OldPawn != NewVehicle)
		{
			OldPawn->Destroy();
		}

		UE_LOG(LogTemp, Display, TEXT("Successfully spawned and possessed vehicle: %s"), *NewVehicle->GetName());
	}
}

bool AVelocityXGameModeBase::TogglePauseGame()
{
	if (bIsGamePaused)
	{
		UnpauseGame();
	}
	else
	{
		PauseGame();
	}

	return bIsGamePaused;
}

void AVelocityXGameModeBase::PauseGame()
{
	if (bIsGamePaused)
	{
		return;
	}

	bIsGamePaused = true;
	PrePauseState = CurrentMatchState;
	SetMatchState(EVelocityXMatchState::Paused);

	UGameplayStatics::SetGamePaused(GetWorld(), true);

	// Update player controllers
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->bShowMouseCursor = true;
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}

	OnGamePauseChanged.Broadcast(true);
	UE_LOG(LogTemp, Display, TEXT("Game Paused."));
}

void AVelocityXGameModeBase::UnpauseGame()
{
	if (!bIsGamePaused)
	{
		return;
	}

	bIsGamePaused = false;
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	// Restore state prior to pausing
	SetMatchState(PrePauseState);

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->bShowMouseCursor = false;
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}

	OnGamePauseChanged.Broadcast(false);
	UE_LOG(LogTemp, Display, TEXT("Game Resumed."));
}

void AVelocityXGameModeBase::StartRace()
{
	SetMatchState(EVelocityXMatchState::Racing);

	AVelocityXGameState* GS = GetVelocityGameState();
	if (GS)
	{
		GS->StartTimer();
	}
}

void AVelocityXGameModeBase::EndRace()
{
	SetMatchState(EVelocityXMatchState::Finished);

	AVelocityXGameState* GS = GetVelocityGameState();
	if (GS)
	{
		GS->PauseTimer();
	}
}

void AVelocityXGameModeBase::ResetSessionTimer()
{
	AVelocityXGameState* GS = GetVelocityGameState();
	if (GS)
	{
		GS->ResetTimer();
	}
}

float AVelocityXGameModeBase::GetSessionTime() const
{
	const AVelocityXGameState* GS = GetVelocityGameState();
	return GS ? GS->GetTrackTime() : 0.0f;
}

FString AVelocityXGameModeBase::GetFormattedSessionTime() const
{
	const AVelocityXGameState* GS = GetVelocityGameState();
	return GS ? GS->GetFormattedTrackTime() : TEXT("00:00.000");
}

AVelocityXGameState* AVelocityXGameModeBase::GetVelocityGameState() const
{
	return GetGameState<AVelocityXGameState>();
}
