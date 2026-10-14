// Copyright VELOCITY X Racing. All Rights Reserved.

#include "VelocityPlayerController.h"
#include "Vehicle/BaseCar.h"
#include "GameInstance/VelocityGameInstance.h"
#include "Components/InputComponent.h"
#include "GameFramework/Pawn.h"

AVelocityPlayerController::AVelocityPlayerController()
{
	ControlledCar = nullptr;
	ThrottleValue = 0.0f;
	SteeringValue = 0.0f;
	BrakeValue = 0.0f;
	bHandbrakeActive = false;
	bIsGamePaused = false;
}

void AVelocityPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ControlledCar = Cast<ABaseCar>(GetPawn());
}

void AVelocityPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateControlledVehicleInput();

	// Feed live telemetry to GameInstance
	if (ControlledCar)
	{
		if (UVelocityGameInstance* GI = Cast<UVelocityGameInstance>(GetGameInstance()))
		{
			GI->UpdateTelemetry(ControlledCar->GetSpeedMPH(), ControlledCar->GetSpeedKPH(), ControlledCar->GetEngineRPM(), ControlledCar->GetCurrentGear(), DeltaTime);
			if (UVelocityTelemetryTracker* Tracker = GI->GetTelemetryTracker())
			{
				Tracker->UpdateInputs(ThrottleValue, BrakeValue, SteeringValue);
			}
		}
	}
}

void AVelocityPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledCar = Cast<ABaseCar>(InPawn);
	if (ControlledCar)
	{
		UE_LOG(LogTemp, Display, TEXT("VelocityPlayerController successfully possessed: %s"), *InPawn->GetName());
	}
}

void AVelocityPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	ControlledCar = nullptr;
}

void AVelocityPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputComponent)
	{
		return;
	}

	// Axis bindings
	InputComponent->BindAxis("Throttle", this, &AVelocityPlayerController::InputThrottleAxis);
	InputComponent->BindAxis("Steering", this, &AVelocityPlayerController::InputSteeringAxis);
	InputComponent->BindAxis("Brake", this, &AVelocityPlayerController::InputBrakeAxis);

	// Action bindings
	InputComponent->BindAction("Handbrake", IE_Pressed, this, &AVelocityPlayerController::ActionHandbrakePressed);
	InputComponent->BindAction("Handbrake", IE_Released, this, &AVelocityPlayerController::ActionHandbrakeReleased);

	InputComponent->BindAction("Reset", IE_Pressed, this, &AVelocityPlayerController::ActionResetCar);
	InputComponent->BindAction("Headlights", IE_Pressed, this, &AVelocityPlayerController::ActionToggleHeadlights);
	InputComponent->BindAction("Camera", IE_Pressed, this, &AVelocityPlayerController::ActionCycleCamera);
	InputComponent->BindAction("Pause", IE_Pressed, this, &AVelocityPlayerController::ActionTogglePause);
}

void AVelocityPlayerController::InputThrottleAxis(float Value)
{
	SetThrottleInput(Value);
}

void AVelocityPlayerController::InputSteeringAxis(float Value)
{
	SetSteeringInput(Value);
}

void AVelocityPlayerController::InputBrakeAxis(float Value)
{
	SetBrakeInput(Value);
}

void AVelocityPlayerController::SetThrottleInput(float Value)
{
	ThrottleValue = FMath::Clamp(Value, 0.0f, 1.0f);
}

void AVelocityPlayerController::SetSteeringInput(float Value)
{
	SteeringValue = FMath::Clamp(Value, -1.0f, 1.0f);
}

void AVelocityPlayerController::SetBrakeInput(float Value)
{
	BrakeValue = FMath::Clamp(Value, 0.0f, 1.0f);
}

void AVelocityPlayerController::SetHandbrakeInput(bool bPressed)
{
	bHandbrakeActive = bPressed;
}

void AVelocityPlayerController::ActionHandbrakePressed()
{
	SetHandbrakeInput(true);
}

void AVelocityPlayerController::ActionHandbrakeReleased()
{
	SetHandbrakeInput(false);
}

void AVelocityPlayerController::ActionResetCar()
{
	if (ControlledCar)
	{
		ControlledCar->ResetVehicle();
	}
}

void AVelocityPlayerController::ActionToggleHeadlights()
{
	if (ControlledCar)
	{
		ControlledCar->ToggleHeadlights();
	}
}

void AVelocityPlayerController::ActionCycleCamera()
{
	if (ControlledCar)
	{
		ControlledCar->CycleCameraMode();
	}
}

void AVelocityPlayerController::ActionTogglePause()
{
	bIsGamePaused = !bIsGamePaused;
	SetPause(bIsGamePaused);
	UE_LOG(LogTemp, Display, TEXT("Game pause toggled: %s"), bIsGamePaused ? TEXT("PAUSED") : TEXT("RESUMED"));
}

void AVelocityPlayerController::UpdateControlledVehicleInput()
{
	if (!ControlledCar)
	{
		// Try resolving controlled pawn if recently spawned
		ControlledCar = Cast<ABaseCar>(GetPawn());
	}

	if (ControlledCar)
	{
		ControlledCar->ApplyThrottle(ThrottleValue);
		ControlledCar->ApplySteering(SteeringValue);
		ControlledCar->ApplyBrake(BrakeValue);
		ControlledCar->ApplyHandbrake(bHandbrakeActive);
	}
}
