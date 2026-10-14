// Copyright VELOCITY X Racing. All Rights Reserved.

#include "VelocityXPlayerController.h"
#include "VelocityXGameModeBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"

const FName AVelocityXPlayerController::SocketChaseCamera = FName("Socket_Chase");
const FName AVelocityXPlayerController::SocketHoodCamera = FName("Socket_Hood");
const FName AVelocityXPlayerController::SocketCockpitCamera = FName("Socket_Cockpit");

AVelocityXPlayerController::AVelocityXPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentThrottle = 0.0f;
	CurrentBrake = 0.0f;
	CurrentSteering = 0.0f;
	TargetThrottle = 0.0f;
	TargetBrake = 0.0f;
	TargetSteering = 0.0f;

	bHandbrakeActive = false;
	bHeadlightsActive = false;

	CurrentCameraMode = EVelocityXCameraMode::Chase;
	CockpitLookOffset = FRotator::ZeroRotator;
	MouseLookSensitivity = 1.0f;
	CockpitLookMaxYaw = 85.0f;
	CockpitLookMaxPitch = 45.0f;

	SteeringInterpSpeed = 12.0f;
	PedalInterpSpeed = 15.0f;

	bShowMouseCursor = false;
}

void AVelocityXPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetDrivingInputMode();
	UE_LOG(LogTemp, Display, TEXT("AVelocityXPlayerController initialized in Driving mode."));
}

void AVelocityXPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Smooth pedal and steering inputs
	CurrentThrottle = FMath::FInterpTo(CurrentThrottle, TargetThrottle, DeltaTime, PedalInterpSpeed);
	CurrentBrake = FMath::FInterpTo(CurrentBrake, TargetBrake, DeltaTime, PedalInterpSpeed);
	CurrentSteering = FMath::FInterpTo(CurrentSteering, TargetSteering, DeltaTime, SteeringInterpSpeed);

	RouteInputsToPossessedPawn(DeltaTime);

	// If not in cockpit view, auto-center mouse look smoothly
	if (CurrentCameraMode != EVelocityXCameraMode::Cockpit)
	{
		CockpitLookOffset = FMath::RInterpTo(CockpitLookOffset, FRotator::ZeroRotator, DeltaTime, 8.0f);
	}

	ApplyCameraModeTransform();
}

void AVelocityXPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputComponent)
	{
		return;
	}

	// --- Vehicle Driving Axis Bindings (WASD / Arrows) ---
	InputComponent->BindAxis(TEXT("Throttle"), this, &AVelocityXPlayerController::InputThrottle);
	InputComponent->BindAxis(TEXT("Brake"), this, &AVelocityXPlayerController::InputBrake);
	InputComponent->BindAxis(TEXT("Steering"), this, &AVelocityXPlayerController::InputSteering);

	// Fallback character mappings if pawn uses MoveForward / MoveRight
	InputComponent->BindAxis(TEXT("MoveForward"), this, &AVelocityXPlayerController::InputThrottle);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &AVelocityXPlayerController::InputSteering);

	// --- Mouse Look for Cockpit View ---
	InputComponent->BindAxis(TEXT("Turn"), this, &AVelocityXPlayerController::InputTurn);
	InputComponent->BindAxis(TEXT("LookUp"), this, &AVelocityXPlayerController::InputLookUp);

	// --- Action Mappings ---
	InputComponent->BindAction(TEXT("Handbrake"), IE_Pressed, this, &AVelocityXPlayerController::InputHandbrakePressed);
	InputComponent->BindAction(TEXT("Handbrake"), IE_Released, this, &AVelocityXPlayerController::InputHandbrakeReleased);

	InputComponent->BindAction(TEXT("Camera"), IE_Pressed, this, &AVelocityXPlayerController::ToggleCameraMode);
	InputComponent->BindAction(TEXT("Headlights"), IE_Pressed, this, &AVelocityXPlayerController::ToggleHeadlights);
	InputComponent->BindAction(TEXT("Reset"), IE_Pressed, this, &AVelocityXPlayerController::ResetVehicleUpright);
	InputComponent->BindAction(TEXT("Pause"), IE_Pressed, this, &AVelocityXPlayerController::TogglePauseMenu);

	// Allow pause key to work even when world is paused
	FInputActionBinding& PauseBinding = InputComponent->BindAction(TEXT("Pause"), IE_Pressed, this, &AVelocityXPlayerController::TogglePauseMenu);
	PauseBinding.bExecuteWhenPaused = true;
}

void AVelocityXPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	SetupCameraComponentsForPawn(InPawn);
	UE_LOG(LogTemp, Display, TEXT("AVelocityXPlayerController possessed: %s"), InPawn ? *InPawn->GetName() : TEXT("None"));
}

void AVelocityXPlayerController::InputThrottle(float Value)
{
	TargetThrottle = FMath::Clamp(Value, 0.0f, 1.0f);
}

void AVelocityXPlayerController::InputBrake(float Value)
{
	TargetBrake = FMath::Clamp(Value, 0.0f, 1.0f);
}

void AVelocityXPlayerController::InputSteering(float Value)
{
	TargetSteering = FMath::Clamp(Value, -1.0f, 1.0f);
}

void AVelocityXPlayerController::InputHandbrakePressed()
{
	bHandbrakeActive = true;
}

void AVelocityXPlayerController::InputHandbrakeReleased()
{
	bHandbrakeActive = false;
}

void AVelocityXPlayerController::ToggleCameraMode()
{
	switch (CurrentCameraMode)
	{
	case EVelocityXCameraMode::Chase:
		SetCameraMode(EVelocityXCameraMode::Hood);
		break;
	case EVelocityXCameraMode::Hood:
		SetCameraMode(EVelocityXCameraMode::Cockpit);
		break;
	case EVelocityXCameraMode::Cockpit:
		SetCameraMode(EVelocityXCameraMode::Chase);
		break;
	}
}

void AVelocityXPlayerController::SetCameraMode(EVelocityXCameraMode NewMode)
{
	CurrentCameraMode = NewMode;
	CockpitLookOffset = FRotator::ZeroRotator;

	ApplyCameraModeTransform();
	OnCameraModeChanged.Broadcast(NewMode);

	UE_LOG(LogTemp, Log, TEXT("Camera mode set to: %d"), static_cast<int32>(NewMode));
}

void AVelocityXPlayerController::ToggleHeadlights()
{
	bHeadlightsActive = !bHeadlightsActive;

	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		// Attempt invocation via reflection / interface
		UFunction* ToggleFunc = ControlledPawn->FindFunction(FName("ToggleHeadlights"));
		if (ToggleFunc)
		{
			ControlledPawn->ProcessEvent(ToggleFunc, nullptr);
		}
	}

	OnHeadlightsToggled.Broadcast(bHeadlightsActive);
	UE_LOG(LogTemp, Log, TEXT("Headlights toggled: %s"), bHeadlightsActive ? TEXT("ON") : TEXT("OFF"));
}

void AVelocityXPlayerController::ResetVehicleUpright()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	// First try native vehicle ResetVehicle()
	UFunction* ResetFunc = ControlledPawn->FindFunction(FName("ResetVehicle"));
	if (ResetFunc)
	{
		ControlledPawn->ProcessEvent(ResetFunc, nullptr);
		UE_LOG(LogTemp, Display, TEXT("Vehicle reset via pawn ResetVehicle() method."));
		return;
	}

	// Physics-based fallback: Level the roll and pitch, raise Z slightly, zero velocity
	FVector CurrentLocation = ControlledPawn->GetActorLocation();
	FRotator CurrentRotation = ControlledPawn->GetActorRotation();

	// Keep Yaw, zero out Roll and Pitch
	FRotator UprightRotation = FRotator(0.0f, CurrentRotation.Yaw, 0.0f);
	FVector UprightLocation = CurrentLocation + FVector(0.0f, 0.0f, 40.0f);

	ControlledPawn->SetActorLocationAndRotation(UprightLocation, UprightRotation, false, nullptr, ETeleportType::ResetPhysics);

	UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(ControlledPawn->GetRootComponent());
	if (RootPrim && RootPrim->IsSimulatingPhysics())
	{
		RootPrim->SetPhysicsLinearVelocity(FVector::ZeroVector);
		RootPrim->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}

	UE_LOG(LogTemp, Display, TEXT("Vehicle reset upright at location: %s"), *UprightLocation.ToString());
}

void AVelocityXPlayerController::TogglePauseMenu()
{
	AVelocityXGameModeBase* GameMode = Cast<AVelocityXGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		const bool bIsPaused = GameMode->TogglePauseGame();
		OnPauseMenuToggled.Broadcast(bIsPaused);
	}
	else
	{
		// Standalone pause fallback
		const bool bWasPaused = IsPaused();
		SetPause(!bWasPaused);
		bShowMouseCursor = !bWasPaused;

		if (!bWasPaused)
		{
			SetMenuInputMode();
		}
		else
		{
			SetDrivingInputMode();
		}

		OnPauseMenuToggled.Broadcast(!bWasPaused);
	}
}

void AVelocityXPlayerController::InputTurn(float Value)
{
	if (FMath::IsNearlyZero(Value))
	{
		return;
	}

	if (CurrentCameraMode == EVelocityXCameraMode::Cockpit)
	{
		CockpitLookOffset.Yaw = FMath::Clamp(CockpitLookOffset.Yaw + (Value * MouseLookSensitivity), -CockpitLookMaxYaw, CockpitLookMaxYaw);
	}
	else
	{
		AddYawInput(Value);
	}
}

void AVelocityXPlayerController::InputLookUp(float Value)
{
	if (FMath::IsNearlyZero(Value))
	{
		return;
	}

	if (CurrentCameraMode == EVelocityXCameraMode::Cockpit)
	{
		// Inverted pitch standard
		CockpitLookOffset.Pitch = FMath::Clamp(CockpitLookOffset.Pitch - (Value * MouseLookSensitivity), -CockpitLookMaxPitch, CockpitLookMaxPitch);
	}
	else
	{
		AddPitchInput(Value);
	}
}

void AVelocityXPlayerController::RequestCarSelection(EVelocityXCarType CarType)
{
	AVelocityXGameModeBase* GameMode = Cast<AVelocityXGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->SelectCar(CarType);
		GameMode->SpawnAndPossessSelectedVehicle(this);
	}
}

void AVelocityXPlayerController::ConfirmCarSelectionAndStartRace()
{
	AVelocityXGameModeBase* GameMode = Cast<AVelocityXGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->StartRace();
		SetDrivingInputMode();
	}
}

void AVelocityXPlayerController::SetDrivingInputMode()
{
	bShowMouseCursor = false;
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void AVelocityXPlayerController::SetMenuInputMode()
{
	bShowMouseCursor = true;
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void AVelocityXPlayerController::RouteInputsToPossessedPawn(float DeltaTime)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	// 1. Try ABaseCar individual setter signatures: ApplyThrottle, ApplyBrake, ApplySteering, ApplyHandbrake
	UFunction* ApplyThrottleFunc = ControlledPawn->FindFunction(FName("ApplyThrottle"));
	UFunction* ApplyBrakeFunc = ControlledPawn->FindFunction(FName("ApplyBrake"));
	UFunction* ApplySteeringFunc = ControlledPawn->FindFunction(FName("ApplySteering"));
	UFunction* ApplyHandbrakeFunc = ControlledPawn->FindFunction(FName("ApplyHandbrake"));

	if (ApplyThrottleFunc && ApplyBrakeFunc && ApplySteeringFunc && ApplyHandbrakeFunc)
	{
		struct FFloatInputParams { float Value; };
		struct FBoolInputParams { bool bActive; };

		FFloatInputParams ThrottleParams{ CurrentThrottle };
		ControlledPawn->ProcessEvent(ApplyThrottleFunc, &ThrottleParams);

		FFloatInputParams BrakeParams{ CurrentBrake };
		ControlledPawn->ProcessEvent(ApplyBrakeFunc, &BrakeParams);

		FFloatInputParams SteerParams{ CurrentSteering };
		ControlledPawn->ProcessEvent(ApplySteeringFunc, &SteerParams);

		FBoolInputParams HandbrakeParams{ bHandbrakeActive };
		ControlledPawn->ProcessEvent(ApplyHandbrakeFunc, &HandbrakeParams);
		return;
	}

	// 2. Try AVELOCITYXVehicle composite signature: ApplyInput(Throttle, Brake, Steering, Handbrake)
	UFunction* ApplyInputFunc = ControlledPawn->FindFunction(FName("ApplyInput"));
	if (ApplyInputFunc)
	{
		struct FCompositeInputParams
		{
			float Throttle;
			float Brake;
			float Steering;
			bool Handbrake;
		};

		FCompositeInputParams Params{ CurrentThrottle, CurrentBrake, CurrentSteering, bHandbrakeActive };
		ControlledPawn->ProcessEvent(ApplyInputFunc, &Params);
		return;
	}

	// 3. Fallback: Generic Character / Pawn movement
	ControlledPawn->AddMovementInput(ControlledPawn->GetActorForwardVector(), CurrentThrottle - CurrentBrake);
	ControlledPawn->AddMovementInput(ControlledPawn->GetActorRightVector(), CurrentSteering);
}

void AVelocityXPlayerController::SetupCameraComponentsForPawn(APawn* InPawn)
{
	if (!InPawn)
	{
		return;
	}

	ApplyCameraModeTransform();
}

void AVelocityXPlayerController::ApplyCameraModeTransform()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	USpringArmComponent* SpringArm = ControlledPawn->FindComponentByClass<USpringArmComponent>();
	UCameraComponent* Camera = ControlledPawn->FindComponentByClass<UCameraComponent>();

	if (!SpringArm && !Camera)
	{
		return;
	}

	switch (CurrentCameraMode)
	{
	case EVelocityXCameraMode::Chase:
		if (SpringArm)
		{
			SpringArm->TargetArmLength = 450.0f;
			SpringArm->SocketOffset = FVector(0.0f, 0.0f, 120.0f);
			SpringArm->TargetOffset = FVector(0.0f, 0.0f, 60.0f);
			SpringArm->bEnableCameraLag = true;
			SpringArm->CameraLagSpeed = 12.0f;
			SpringArm->SetRelativeRotation(FRotator(-8.0f, 0.0f, 0.0f));
		}
		if (Camera)
		{
			Camera->SetRelativeRotation(FRotator::ZeroRotator);
			Camera->FieldOfView = 90.0f;
		}
		break;

	case EVelocityXCameraMode::Hood:
		if (SpringArm)
		{
			SpringArm->TargetArmLength = 0.0f;
			SpringArm->SocketOffset = FVector(180.0f, 0.0f, 75.0f);
			SpringArm->TargetOffset = FVector::ZeroVector;
			SpringArm->bEnableCameraLag = false;
			SpringArm->SetRelativeRotation(FRotator(-3.0f, 0.0f, 0.0f));
		}
		if (Camera)
		{
			Camera->SetRelativeRotation(FRotator::ZeroRotator);
			Camera->FieldOfView = 95.0f;
		}
		break;

	case EVelocityXCameraMode::Cockpit:
		if (SpringArm)
		{
			SpringArm->TargetArmLength = 0.0f;
			SpringArm->SocketOffset = FVector(-15.0f, -38.0f, 95.0f); // Driver seat offset
			SpringArm->TargetOffset = FVector::ZeroVector;
			SpringArm->bEnableCameraLag = false;
			SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
		}
		if (Camera)
		{
			// Apply mouse look free-look in cockpit
			Camera->SetRelativeRotation(CockpitLookOffset);
			Camera->FieldOfView = 82.0f;
		}
		break;
	}
}
