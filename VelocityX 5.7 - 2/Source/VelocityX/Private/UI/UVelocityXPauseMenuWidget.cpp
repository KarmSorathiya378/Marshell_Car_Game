// Copyright VELOCITY X Racing. All Rights Reserved.

#include "UVelocityXPauseMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

UVelocityXPauseMenuWidget::UVelocityXPauseMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsFocusable = true;
}

void UVelocityXPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Bind Resume
	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UVelocityXPauseMenuWidget::HandleResumeClicked);
		ResumeButton->OnHovered.AddDynamic(this, &UVelocityXPauseMenuWidget::HandleHover);
	}

	// Bind Restart
	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UVelocityXPauseMenuWidget::HandleRestartClicked);
		RestartButton->OnHovered.AddDynamic(this, &UVelocityXPauseMenuWidget::HandleHover);
	}

	// Bind Settings
	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &UVelocityXPauseMenuWidget::HandleSettingsClicked);
		SettingsButton->OnHovered.AddDynamic(this, &UVelocityXPauseMenuWidget::HandleHover);
	}

	// Bind Quit To Menu
	if (QuitToMenuButton)
	{
		QuitToMenuButton->OnClicked.AddDynamic(this, &UVelocityXPauseMenuWidget::HandleQuitToMenuClicked);
		QuitToMenuButton->OnHovered.AddDynamic(this, &UVelocityXPauseMenuWidget::HandleHover);
	}
}

void UVelocityXPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PauseOpenSound)
	{
		UGameplayStatics::PlaySound2D(this, PauseOpenSound);
	}

	// Pause game simulation
	UGameplayStatics::SetGamePaused(this, true);

	// Setup input mode
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
	}

	if (ResumeButton)
	{
		ResumeButton->SetKeyboardFocus();
	}
}

void UVelocityXPauseMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UVelocityXPauseMenuWidget::SetRaceTelemetry(float CurrentRaceSeconds, const FString& VehicleName)
{
	if (CurrentTimeText)
	{
		int32 Minutes = FMath::FloorToInt(CurrentRaceSeconds / 60.0f);
		int32 Seconds = FMath::FloorToInt(FMath::Fmod(CurrentRaceSeconds, 60.0f));
		int32 Millis  = FMath::FloorToInt(FMath::Frac(CurrentRaceSeconds) * 1000.0f);

		FString TimeStr = FString::Printf(TEXT("%02d:%02d.%03d"), Minutes, Seconds, Millis);
		CurrentTimeText->SetText(FText::FromString(TimeStr));
	}

	if (VehicleModelText)
	{
		VehicleModelText->SetText(FText::FromString(VehicleName));
	}
}

void UVelocityXPauseMenuWidget::ResumeGame()
{
	if (PauseResumeSound)
	{
		UGameplayStatics::PlaySound2D(this, PauseResumeSound);
	}

	// Unpause game
	UGameplayStatics::SetGamePaused(this, false);

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}

	RemoveFromParent();
}

void UVelocityXPauseMenuWidget::HandleResumeClicked()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound);
	}

	UE_LOG(LogTemp, Display, TEXT("[VelocityX UI] Pause Menu: Resume Clicked."));
	OnResumeClicked.Broadcast();
	ResumeGame();
}

void UVelocityXPauseMenuWidget::HandleRestartClicked()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound);
	}

	UE_LOG(LogTemp, Display, TEXT("[VelocityX UI] Pause Menu: Restart Race Clicked."));
	OnRestartClicked.Broadcast();

	// Unpause before restart
	UGameplayStatics::SetGamePaused(this, false);

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}

	RemoveFromParent();
}

void UVelocityXPauseMenuWidget::HandleSettingsClicked()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound);
	}

	UE_LOG(LogTemp, Display, TEXT("[VelocityX UI] Pause Menu: Settings Clicked."));
	OnSettingsClicked.Broadcast();
}

void UVelocityXPauseMenuWidget::HandleQuitToMenuClicked()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound);
	}

	UE_LOG(LogTemp, Display, TEXT("[VelocityX UI] Pause Menu: Quit To Menu Clicked."));
	OnQuitToMenuClicked.Broadcast();

	// Unpause before level change
	UGameplayStatics::SetGamePaused(this, false);

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		FInputModeUIOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
	}

	UGameplayStatics::OpenLevel(this, FName("MainMenu_Map"));
}

void UVelocityXPauseMenuWidget::HandleHover()
{
	if (HoverSound)
	{
		UGameplayStatics::PlaySound2D(this, HoverSound);
	}
}
