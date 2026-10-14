// Copyright VELOCITY X Racing. All Rights Reserved.

#include "VelocityPlayerController.h"
#include "VelocityCameraManager.h"
#include "VELOCITYXVehicle.h"
#include "Vehicle/BaseCar.h"
#include "VELOCITYXGameMode.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/World.h"

AVelocityPlayerController::AVelocityPlayerController()
{
	PlayerCameraManagerClass = AVelocityCameraManager::StaticClass();

	// Settings & Defaults
	SteeringDeadzone = 0.05f;
	SteeringSensitivity = 1.0f;
	SteeringLinearity = 1.25f; // Slight exponential curve for fine high-speed adjustments
	TriggerDeadzone = 0.02f;
	ThrottleSensitivity = 1.0f;
	BrakeSensitivity = 1.0f;

	bEnableGamepadVibration = true;
	HandbrakeRumbleIntensity = 0.65f;

	bEnableMouseSteeringAssist = false;
	MouseSteeringSensitivity = 0.015f;
	MouseSteeringAutoCenterRate = 4.0f;

	RawGamepadThrottle = 0.0f;
	RawGamepadBrake = 0.0f;
	RawGamepadSteer = 0.0f;
	MouseSteerAccumulator = 0.0f;
	bIsHandbrakeRumbling = false;
	ABSPulseTimer = 0.0f;

	CameraIndex = 0;
}

void AVelocityPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputModeDriving();
	ResolveTargetVehicles();
	UpdateCameraManagerReference();
}

void AVelocityPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ResolveTargetVehicles();
	UpdateCameraManagerReference();
	ProcessVehicleInputs(DeltaTime);

	// Handbrake continuous haptics
	if (bHandbrakePressed)
	{
		float CarSpeed = 0.0f;
		if (TargetBaseCar)
		{
			CarSpeed = TargetBaseCar->GetSpeedMPH();
		}
		else if (CurrentVehicle)
		{
			CarSpeed = CurrentVehicle->GetSpeedMPH();
		}

		if (CarSpeed > 5.0f)
		{
			PlayHandbrakeHaptics(DeltaTime);
		}
	}
	else if (bIsHandbrakeRumbling)
	{
		StopAllHaptics();
	}
}

void AVelocityPlayerController::ResolveTargetVehicles()
{
	APawn* ControlledPawn = GetPawn();
	if (!TargetBaseCar && ControlledPawn)
	{
		TargetBaseCar = Cast<ABaseCar>(ControlledPawn);
	}
	if (!CurrentVehicle && ControlledPawn)
	{
		CurrentVehicle = Cast<AVELOCITYXVehicle>(ControlledPawn);
	}
}

void AVelocityPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		// Gamepad analog axis bindings
		InputComponent->BindAxis(TEXT("ThrottleAxis"), this, &AVelocityPlayerController::HandleGamepadAnalogThrottle);
		InputComponent->BindAxis(TEXT("BrakeAxis"), this, &AVelocityPlayerController::HandleGamepadAnalogBrake);
		InputComponent->BindAxis(TEXT("Steering"), this, &AVelocityPlayerController::HandleGamepadAnalogSteer);
		InputComponent->BindAxis(TEXT("MouseSteerAxis"), this, &AVelocityPlayerController::HandleGamepadAnalogSteer);

		// D-Pad camera shortcuts
		InputComponent->BindAction(TEXT("CameraChase"), IE_Pressed, this, &AVelocityPlayerController::HandleGamepadQuickCameraChase);
		InputComponent->BindAction(TEXT("CameraCockpit"), IE_Pressed, this, &AVelocityPlayerController::HandleGamepadQuickCameraCockpit);

		// Shifting actions
		InputComponent->BindAction(TEXT("ShiftUp"), IE_Pressed, this, &AVelocityPlayerController::ShiftUp);
		InputComponent->BindAction(TEXT("ShiftDown"), IE_Pressed, this, &AVelocityPlayerController::ShiftDown);

		// Pause action executes even when paused
		FInputActionBinding& PauseBinding = InputComponent->BindAction(TEXT("Pause"), IE_Pressed, this, &AVelocityPlayerController::TogglePause);
		PauseBinding.bExecuteWhenPaused = true;
	}

	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent))
	{
		BindEnhancedInputActions(EnhancedInputComp);
	}
}

void AVelocityPlayerController::BindEnhancedInputActions(UEnhancedInputComponent* EnhancedInputComp)
{
	if (!EnhancedInputComp || !InputConfig)
	{
		return;
	}

	// Driving Controls
	if (InputConfig->IA_Throttle)
	{
		EnhancedInputComp->BindAction(InputConfig->IA_Throttle, ETriggerEvent::Triggered, this, &AVelocityPlayerController::HandleEnhancedThrottle);
		EnhancedInputComp->BindAction(InputConfig->IA_Throttle, ETriggerEvent::Completed, this, &AVelocityPlayerController::HandleEnhancedThrottleCompleted);
	}

	if (InputConfig->IA_Brake)
	{
		EnhancedInputComp->BindAction(InputConfig->IA_Brake, ETriggerEvent::Triggered, this, &AVelocityPlayerController::HandleEnhancedBrake);
		EnhancedInputComp->BindAction(InputConfig->IA_Brake, ETriggerEvent::Completed, this, &AVelocityPlayerController::HandleEnhancedBrakeCompleted);
	}

	if (InputConfig->IA_Steer)
	{
		EnhancedInputComp->BindAction(InputConfig->IA_Steer, ETriggerEvent::Triggered, this, &AVelocityPlayerController::HandleEnhancedSteer);
		EnhancedInputComp->BindAction(InputConfig->IA_Steer, ETriggerEvent::Completed, this, &AVelocityPlayerController::HandleEnhancedSteerCompleted);
	}

	if (InputConfig->IA_Handbrake)
	{
		EnhancedInputComp->BindAction(InputConfig->IA_Handbrake, ETriggerEvent::Started, this, &AVelocityPlayerController::HandleEnhancedHandbrake);
		EnhancedInputComp->BindAction(InputConfig->IA_Handbrake, ETriggerEvent::Completed, this, &AVelocityPlayerController::HandleEnhancedHandbrakeCompleted);
	}

	// Manual Shifting
	if (InputConfig->IA_ShiftUp)
	{
		EnhancedInputComp->BindAction(InputConfig->IA_ShiftUp, ETriggerEvent::Triggered, this, &AVelocityPlayerController::ShiftUp);
	}
	if (InputConfig->IA_ShiftDown)
	{
		EnhancedInputComp->BindAction(InputConfig->IA_ShiftDown, ETriggerEvent::Triggered, this, &AVelocityPlayerController::ShiftDown);
	}

	// Camera & Utilities
	if (InputConfig->IA_CycleCamera)
	{
		EnhancedInputComp->BindAction(InputConfig->IA_CycleCamera, ETriggerEvent::Triggered, this, &AVelocityPlayerController::CycleCamera);
	}

	if (InputConfig->IA_ToggleHeadlights)
	{
		EnhancedInputComp->BindAction(InputConfig->IA_ToggleHeadlights, ETriggerEvent::Triggered, this, &AVelocityPlayerController::ToggleHeadlights);
	}

	if (InputConfig->IA_ResetCar)
	{
		EnhancedInputComp->BindAction(InputConfig->IA_ResetCar, ETriggerEvent::Triggered, this, &AVelocityPlayerController::ResetCar);
	}

	// Pause Action
	if (InputConfig->IA_Pause)
	{
		EnhancedInputComp->BindAction(InputConfig->IA_Pause, ETriggerEvent::Triggered, this, &AVelocityPlayerController::TogglePause).bExecuteWhenPaused = true;
	}

	if (InputConfig->IA_MouseSteer)
	{
		EnhancedInputComp->BindAction(InputConfig->IA_MouseSteer, ETriggerEvent::Triggered, this, &AVelocityPlayerController::HandleEnhancedMouseSteer);
	}

	UE_LOG(LogTemp, Display, TEXT("[VelocityInput] Enhanced Input bindings configured successfully."));
}

void AVelocityPlayerController::SetInputModeDriving()
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputConfig && InputConfig->MenuMappingContext)
			{
				Subsystem->RemoveMappingContext(InputConfig->MenuMappingContext);
			}
			if (InputConfig && InputConfig->PauseMappingContext)
			{
				Subsystem->RemoveMappingContext(InputConfig->PauseMappingContext);
			}
			if (InputConfig && InputConfig->DrivingMappingContext)
			{
				Subsystem->AddMappingContext(InputConfig->DrivingMappingContext, InputConfig->DrivingContextPriority);
			}
			else if (InputConfig && InputConfig->DefaultMappingContext)
			{
				Subsystem->AddMappingContext(InputConfig->DefaultMappingContext, 0);
			}
		}
	}

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
	UE_LOG(LogTemp, Display, TEXT("[VelocityInput] Switched context to IMC_Driving (GameOnly)"));
}

void AVelocityPlayerController::SetInputModeUI()
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputConfig && InputConfig->DrivingMappingContext)
			{
				Subsystem->RemoveMappingContext(InputConfig->DrivingMappingContext);
			}
			if (InputConfig && InputConfig->MenuMappingContext)
			{
				Subsystem->AddMappingContext(InputConfig->MenuMappingContext, InputConfig->MenuContextPriority);
			}
		}
	}

	bShowMouseCursor = true;
	SetInputMode(FInputModeGameAndUI());
	StopAllHaptics();
	UE_LOG(LogTemp, Display, TEXT("[VelocityInput] Switched context to IMC_Menu (GameAndUI)"));
}

void AVelocityPlayerController::SetInputModePause()
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputConfig && InputConfig->DrivingMappingContext)
			{
				Subsystem->RemoveMappingContext(InputConfig->DrivingMappingContext);
			}
			if (InputConfig && InputConfig->PauseMappingContext)
			{
				Subsystem->AddMappingContext(InputConfig->PauseMappingContext, InputConfig->PauseContextPriority);
			}
			else if (InputConfig && InputConfig->MenuMappingContext)
			{
				Subsystem->AddMappingContext(InputConfig->MenuMappingContext, InputConfig->MenuContextPriority);
			}
		}
	}

	bShowMouseCursor = true;
	SetInputMode(FInputModeGameAndUI());
	StopAllHaptics();
	UE_LOG(LogTemp, Display, TEXT("[VelocityInput] Switched context to IMC_Pause (GameAndUI)"));
}

void AVelocityPlayerController::ShiftUp()
{
	if (TargetBaseCar)
	{
		// Zelda's ABaseCar shifting
		TargetBaseCar->ApplyThrottle(ThrottleInput);
		UE_LOG(LogTemp, Display, TEXT("[VelocityTransmission] Shift Up requested on ABaseCar"));
	}
}

void AVelocityPlayerController::ShiftDown()
{
	if (TargetBaseCar)
	{
		UE_LOG(LogTemp, Display, TEXT("[VelocityTransmission] Shift Down requested on ABaseCar"));
	}
}

void AVelocityPlayerController::CycleCamera()
{
	CameraIndex++;
	if (CameraIndex >= 4)
	{
		CameraIndex = 0;
	}

	if (VelocityCameraManager)
	{
		VelocityCameraManager->CycleCameraMode();
	}

	UE_LOG(LogTemp, Display, TEXT("[VelocityCamera] Cycled Camera to Mode %d"), CameraIndex);
}

void AVelocityPlayerController::SetCamera(int Index)
{
	CameraIndex = FMath::Clamp(Index, 0, 3);
	if (VelocityCameraManager)
	{
		VelocityCameraManager->SetCameraMode(static_cast<EVelocityCameraMode>(CameraIndex), false);
	}
}

void AVelocityPlayerController::TogglePause()
{
	bPaused = !bPaused;
	SetPause(bPaused);

	if (bPaused)
	{
		SetInputModePause();
	}
	else
	{
		SetInputModeDriving();
	}

	// Notify GameMode
	if (AVELOCITYXGameMode* GM = Cast<AVELOCITYXGameMode>(GetWorld() ? GetWorld()->GetAuthGameMode() : nullptr))
	{
		GM->SetGameState(bPaused ? EGameState::Paused : EGameState::Racing);
	}

	// Broadcast delegate
	if (OnPauseToggled.IsBound())
	{
		OnPauseToggled.Broadcast(bPaused);
	}
}

void AVelocityPlayerController::ResetCar()
{
	ResolveTargetVehicles();

	APawn* PawnToReset = TargetBaseCar ? Cast<APawn>(TargetBaseCar) : (CurrentVehicle ? Cast<APawn>(CurrentVehicle) : GetPawn());
	if (PawnToReset)
	{
		FRotator NewRot = PawnToReset->GetActorRotation();
		NewRot.Pitch = 0.0f;
		NewRot.Roll = 0.0f;

		FVector NewLoc = PawnToReset->GetActorLocation();
		NewLoc.Z += 60.0f;

		PawnToReset->SetActorLocationAndRotation(NewLoc, NewRot, false, nullptr, ETeleportType::TeleportPhysics);

		if (TargetBaseCar)
		{
			TargetBaseCar->ResetVehicle();
			TargetBaseCar->ApplyThrottle(0.0f);
			TargetBaseCar->ApplyBrake(0.0f);
			TargetBaseCar->ApplySteering(0.0f);
			TargetBaseCar->ApplyHandbrake(false);
		}
		if (CurrentVehicle)
		{
			CurrentVehicle->ResetVehicle();
			CurrentVehicle->ApplyInput(0.0f, 0.0f, 0.0f, false);
		}

		if (VelocityCameraManager)
		{
			VelocityCameraManager->ResetCamera();
		}

		TriggerCrashHaptics(1.5f);
		UE_LOG(LogTemp, Display, TEXT("[VelocityCar] Vehicle reset complete at %s"), *NewLoc.ToString());
	}

	bThrottlePressed = false;
	bBrakePressed = false;
	bLeftPressed = false;
	bRightPressed = false;
	bHandbrakePressedInput = false;
	RawGamepadThrottle = 0.0f;
	RawGamepadBrake = 0.0f;
	RawGamepadSteer = 0.0f;
	MouseSteerAccumulator = 0.0f;
}

void AVelocityPlayerController::ToggleHeadlights()
{
	Super::ToggleHeadlights();
	if (TargetBaseCar)
	{
		TargetBaseCar->ToggleHeadlights();
	}
	if (CurrentVehicle)
	{
		CurrentVehicle->ToggleHeadlights();
	}
}

void AVelocityPlayerController::HandleEnhancedThrottle(const FInputActionValue& Value)
{
	const float FloatVal = Value.Get<float>();
	bThrottlePressed = FloatVal > 0.1f;
	RawGamepadThrottle = FloatVal * ThrottleSensitivity;
}

void AVelocityPlayerController::HandleEnhancedThrottleCompleted(const FInputActionValue& Value)
{
	bThrottlePressed = false;
	RawGamepadThrottle = 0.0f;
}

void AVelocityPlayerController::HandleEnhancedBrake(const FInputActionValue& Value)
{
	const float FloatVal = Value.Get<float>();
	bBrakePressed = FloatVal > 0.1f;
	RawGamepadBrake = FloatVal * BrakeSensitivity;
}

void AVelocityPlayerController::HandleEnhancedBrakeCompleted(const FInputActionValue& Value)
{
	bBrakePressed = false;
	RawGamepadBrake = 0.0f;
}

void AVelocityPlayerController::HandleEnhancedSteer(const FInputActionValue& Value)
{
	RawGamepadSteer = ApplySteeringCurve(Value.Get<float>());
}

void AVelocityPlayerController::HandleEnhancedSteerCompleted(const FInputActionValue& Value)
{
	RawGamepadSteer = 0.0f;
}

void AVelocityPlayerController::HandleEnhancedHandbrake(const FInputActionValue& Value)
{
	bHandbrakePressedInput = true;
}

void AVelocityPlayerController::HandleEnhancedHandbrakeCompleted(const FInputActionValue& Value)
{
	bHandbrakePressedInput = false;
}

void AVelocityPlayerController::HandleEnhancedMouseSteer(const FInputActionValue& Value)
{
	if (!bEnableMouseSteeringAssist)
	{
		return;
	}

	const float MouseDeltaX = Value.Get<float>();
	MouseSteerAccumulator = FMath::Clamp(MouseSteerAccumulator + (MouseDeltaX * MouseSteeringSensitivity), -1.0f, 1.0f);
}

void AVelocityPlayerController::HandleGamepadAnalogThrottle(float Value)
{
	RawGamepadThrottle = (Value > TriggerDeadzone) ? (Value * ThrottleSensitivity) : 0.0f;
}

void AVelocityPlayerController::HandleGamepadAnalogBrake(float Value)
{
	RawGamepadBrake = (Value > TriggerDeadzone) ? (Value * BrakeSensitivity) : 0.0f;
}

void AVelocityPlayerController::HandleGamepadAnalogSteer(float Value)
{
	RawGamepadSteer = ApplySteeringCurve(Value);
}

void AVelocityPlayerController::HandleGamepadQuickCameraChase()
{
	SetCamera(0);
}

void AVelocityPlayerController::HandleGamepadQuickCameraCockpit()
{
	SetCamera(2);
}

float AVelocityPlayerController::ApplySteeringCurve(float RawInput) const
{
	const float AbsVal = FMath::Abs(RawInput);
	if (AbsVal <= SteeringDeadzone)
	{
		return 0.0f;
	}

	const float Normalized = (AbsVal - SteeringDeadzone) / (1.0f - SteeringDeadzone);
	const float Curved = FMath::Pow(Normalized, SteeringLinearity) * SteeringSensitivity;
	return FMath::Clamp(FMath::Sign(RawInput) * Curved, -1.0f, 1.0f);
}

void AVelocityPlayerController::ApplyControlSettings(float InSteerSens, float InSteerDeadzone, float InSteerLinearity, float InTriggerDeadzone)
{
	SteeringSensitivity = FMath::Clamp(InSteerSens, 0.2f, 3.0f);
	SteeringDeadzone = FMath::Clamp(InSteerDeadzone, 0.0f, 0.35f);
	SteeringLinearity = FMath::Clamp(InSteerLinearity, 0.5f, 2.5f);
	TriggerDeadzone = FMath::Clamp(InTriggerDeadzone, 0.0f, 0.25f);
	UE_LOG(LogTemp, Display, TEXT("[VelocitySettings] Control settings applied: Sens=%.2f, Deadzone=%.2f, Linearity=%.2f"),
		SteeringSensitivity, SteeringDeadzone, SteeringLinearity);
}

void AVelocityPlayerController::PlayHandbrakeHaptics(float DeltaTime)
{
	if (!bEnableGamepadVibration)
	{
		return;
	}

	bIsHandbrakeRumbling = true;
	PlayDynamicForceFeedback(HandbrakeRumbleIntensity, 0.1f, true, false, true, false, EDynamicForceFeedbackAction::Start);
}

void AVelocityPlayerController::PlayTireSlipHaptics(float SlipRatio)
{
	if (!bEnableGamepadVibration)
	{
		return;
	}

	const float Intensity = FMath::Clamp(SlipRatio * 0.45f, 0.1f, 0.55f);
	PlayDynamicForceFeedback(Intensity, 0.08f, false, true, false, true, EDynamicForceFeedbackAction::Start);
}

void AVelocityPlayerController::PlayRedlineHaptics(float RPMNormalized)
{
	if (!bEnableGamepadVibration || RPMNormalized < 0.92f)
	{
		return;
	}

	const float Intensity = FMath::Clamp((RPMNormalized - 0.92f) / 0.08f, 0.2f, 0.7f);
	PlayDynamicForceFeedback(Intensity, 0.06f, true, true, false, false, EDynamicForceFeedbackAction::Start);
}

void AVelocityPlayerController::PlayHardBrakingABSHaptics(float BrakeIntensity)
{
	if (!bEnableGamepadVibration || BrakeIntensity < 0.85f)
	{
		return;
	}

	ABSPulseTimer += GetWorld()->GetDeltaSeconds();
	const float Pulse = (FMath::Sin(ABSPulseTimer * 40.0f) > 0.0f) ? 0.6f : 0.1f;
	PlayDynamicForceFeedback(Pulse, 0.05f, true, false, false, false, EDynamicForceFeedbackAction::Start);
}

void AVelocityPlayerController::PlayCollisionHaptics(float ImpactForce)
{
	TriggerCrashHaptics(ImpactForce);
}

void AVelocityPlayerController::TriggerCrashHaptics(float Intensity)
{
	if (!bEnableGamepadVibration)
	{
		return;
	}

	PlayDynamicForceFeedback(FMath::Clamp(Intensity, 0.5f, 1.0f), 0.35f, true, true, true, true, EDynamicForceFeedbackAction::Start);

	if (VelocityCameraManager)
	{
		VelocityCameraManager->TriggerCrashShake(Intensity);
	}
}

void AVelocityPlayerController::StopAllHaptics()
{
	bIsHandbrakeRumbling = false;
	PlayDynamicForceFeedback(0.0f, 0.01f, false, false, false, false, EDynamicForceFeedbackAction::Stop);
}

void AVelocityPlayerController::ProcessVehicleInputs(float DeltaTime)
{
	// 1. Throttle: Combine digital and analog
	ThrottleInput = FMath::Clamp(FMath::Max(bThrottlePressed ? 1.0f : 0.0f, RawGamepadThrottle), 0.0f, 1.0f);

	// 2. Brake: Combine digital and analog
	BrakeInput = FMath::Clamp(FMath::Max(bBrakePressed ? 1.0f : 0.0f, RawGamepadBrake), 0.0f, 1.0f);

	// 3. Handbrake
	bHandbrakePressed = bHandbrakePressedInput;

	// 4. Steering: Combine keyboard, analog gamepad, and mouse assist
	float TargetDigitalSteer = 0.0f;
	if (bLeftPressed && !bRightPressed)
	{
		TargetDigitalSteer = -1.0f;
	}
	else if (bRightPressed && !bLeftPressed)
	{
		TargetDigitalSteer = 1.0f;
	}

	if (FMath::Abs(TargetDigitalSteer) > 0.1f)
	{
		SteeringInput = FMath::FInterpTo(SteeringInput, TargetDigitalSteer, DeltaTime, 12.0f);
	}
	else if (FMath::Abs(RawGamepadSteer) > 0.005f)
	{
		SteeringInput = RawGamepadSteer;
	}
	else if (bEnableMouseSteeringAssist)
	{
		MouseSteerAccumulator = FMath::FInterpTo(MouseSteerAccumulator, 0.0f, DeltaTime, MouseSteeringAutoCenterRate);
		SteeringInput = MouseSteerAccumulator;
	}
	else
	{
		SteeringInput = FMath::FInterpTo(SteeringInput, 0.0f, DeltaTime, 14.0f);
	}

	SteeringInput = FMath::Clamp(SteeringInput, -1.0f, 1.0f);

	// Route directly to Zelda's ABaseCar
	if (TargetBaseCar)
	{
		TargetBaseCar->ApplyThrottle(ThrottleInput);
		TargetBaseCar->ApplyBrake(BrakeInput);
		TargetBaseCar->ApplySteering(SteeringInput);
		TargetBaseCar->ApplyHandbrake(bHandbrakePressed);
	}

	// Route to CurrentVehicle
	if (CurrentVehicle)
	{
		CurrentVehicle->ApplyInput(ThrottleInput, BrakeInput, SteeringInput, bHandbrakePressed);
	}
}

void AVelocityPlayerController::UpdateCameraManagerReference()
{
	if (!VelocityCameraManager)
	{
		VelocityCameraManager = Cast<AVelocityCameraManager>(PlayerCameraManager);
	}

	if (VelocityCameraManager)
	{
		if (TargetBaseCar && VelocityCameraManager->GetTargetPawn() != TargetBaseCar)
		{
			VelocityCameraManager->SetTargetBaseCar(TargetBaseCar);
		}
		else if (CurrentVehicle && VelocityCameraManager->GetTargetPawn() != CurrentVehicle)
		{
			VelocityCameraManager->SetTargetVehicle(CurrentVehicle);
		}
	}
}
