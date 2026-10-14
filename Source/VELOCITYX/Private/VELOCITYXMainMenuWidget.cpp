// Copyright VELOCITY X Racing. All Rights Reserved.

#include "VELOCITYXMainMenuWidget.h"

void UVELOCITYXMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button events
	if (PlayButton)
	{
		PlayButton->OnClicked.AddDynamic(this, &UVELOCITYXMainMenuWidget::OnPlayButtonClicked);
	}

	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &UVELOCITYXMainMenuWidget::OnSettingsButtonClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UVELOCITYXMainMenuWidget::OnQuitButtonClicked);
	}

	if (CreditsButton)
	{
		CreditsButton->OnClicked.AddDynamic(this, &UVELOCITYXMainMenuWidget::OnCreditsButtonClicked);
	}
}

void UVELOCITYXMainMenuWidget::OnPlayButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("Play button clicked"));

	// Transition to car selection
	OnPlayButtonClickedDelegate.Broadcast();
}

void UVELOCITYXMainMenuWidget::OnSettingsButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("Settings button clicked"));
	OnSettingsButtonClickedDelegate.Broadcast();
}

void UVELOCITYXMainMenuWidget::OnQuitButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("Quit button clicked"));
	OnQuitButtonClickedDelegate.Broadcast();
}

void UVELOCITYXMainMenuWidget::OnCreditsButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("Credits button clicked"));
	OnCreditsButtonClickedDelegate.Broadcast();
}

void UVELOCITYXMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UVELOCITYXMainMenuWidget::NativeOnClosed()
{
	Super::NativeOnClosed();
}
