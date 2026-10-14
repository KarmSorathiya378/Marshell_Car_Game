// Copyright VELOCITY X Racing. All Rights Reserved.

#include "UVelocityXMainMenuWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

const FLinearColor UVelocityXMainMenuWidget::ColorObsidianBlack(0.04f, 0.06f, 0.09f, 0.95f);
const FLinearColor UVelocityXMainMenuWidget::ColorNeonCyan(0.0f, 0.9f, 1.0f, 1.0f);
const FLinearColor UVelocityXMainMenuWidget::ColorVelocityCrimson(1.0f, 0.1f, 0.2f, 1.0f);
const FLinearColor UVelocityXMainMenuWidget::ColorPlatinumWhite(0.95f, 0.98f, 1.0f, 1.0f);

UVelocityXMainMenuWidget::UVelocityXMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVelocityXMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Bind Play Button
	if (PlayButton)
	{
		PlayButton->OnClicked.AddDynamic(this, &UVelocityXMainMenuWidget::HandlePlayClicked);
		PlayButton->OnHovered.AddDynamic(this, &UVelocityXMainMenuWidget::HandleGenericHover);
	}

	// Bind Car Select Button
	if (CarSelectButton)
	{
		CarSelectButton->OnClicked.AddDynamic(this, &UVelocityXMainMenuWidget::HandleCarSelectClicked);
		CarSelectButton->OnHovered.AddDynamic(this, &UVelocityXMainMenuWidget::HandleGenericHover);
	}

	// Bind Settings Button
	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &UVelocityXMainMenuWidget::HandleSettingsClicked);
		SettingsButton->OnHovered.AddDynamic(this, &UVelocityXMainMenuWidget::HandleGenericHover);
	}

	// Bind Quit Button
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UVelocityXMainMenuWidget::HandleQuitClicked);
		QuitButton->OnHovered.AddDynamic(this, &UVelocityXMainMenuWidget::HandleGenericHover);
	}
}

void UVelocityXMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ApplyNeonStyling();

	// Set focus for keyboard / gamepad navigation
	if (PlayButton)
	{
		PlayButton->SetKeyboardFocus();
	}
}

void UVelocityXMainMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UVelocityXMainMenuWidget::ApplyNeonStyling()
{
	// Configure typography styling
	if (TitleText)
	{
		TitleText->SetColorAndOpacity(FSlateColor(ColorPlatinumWhite));
		TitleText->SetShadowColorAndOpacity(ColorVelocityCrimson);
		TitleText->SetShadowOffset(FVector2D(2.0f, 2.0f));
	}

	if (SubtitleText)
	{
		SubtitleText->SetColorAndOpacity(FSlateColor(ColorNeonCyan));
	}

	if (TitleLogo)
	{
		TitleLogo->SetColorAndOpacity(FLinearColor::White);
	}
}

void UVelocityXMainMenuWidget::SetCareerStats(int32 CompletedRaces, float TotalPlayTimeSeconds, const FString& BestLapTimeStr)
{
	if (CompletedRacesText)
	{
		CompletedRacesText->SetText(FText::Format(NSLOCTEXT("VelocityX", "RacesFormat", "RACES: {0}"), FText::AsNumber(CompletedRaces)));
	}

	if (PlayTimeText)
	{
		int32 Hours = FMath::FloorToInt(TotalPlayTimeSeconds / 3600.0f);
		int32 Mins = FMath::FloorToInt(FMath::Fmod(TotalPlayTimeSeconds, 3600.0f) / 60.0f);
		FString TimeStr = FString::Printf(TEXT("TIME: %02dh %02dm"), Hours, Mins);
		PlayTimeText->SetText(FText::FromString(TimeStr));
	}

	if (BestRecordText)
	{
		BestRecordText->SetText(FText::Format(NSLOCTEXT("VelocityX", "RecordFormat", "RECORD: {0}"), FText::FromString(BestLapTimeStr)));
	}
}

void UVelocityXMainMenuWidget::HandlePlayClicked()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound);
	}

	UE_LOG(LogTemp, Display, TEXT("[VelocityX UI] Main Menu: Start Race clicked."));
	OnPlayClicked.Broadcast();
}

void UVelocityXMainMenuWidget::HandleCarSelectClicked()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound);
	}

	UE_LOG(LogTemp, Display, TEXT("[VelocityX UI] Main Menu: Car Select clicked."));
	OnCarSelectClicked.Broadcast();
}

void UVelocityXMainMenuWidget::HandleSettingsClicked()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound);
	}

	UE_LOG(LogTemp, Display, TEXT("[VelocityX UI] Main Menu: Settings clicked."));
	OnSettingsClicked.Broadcast();
}

void UVelocityXMainMenuWidget::HandleQuitClicked()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound);
	}

	UE_LOG(LogTemp, Display, TEXT("[VelocityX UI] Main Menu: Quit clicked."));
	OnQuitClicked.Broadcast();

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
	}
}

void UVelocityXMainMenuWidget::HandleGenericHover()
{
	if (HoverSound)
	{
		UGameplayStatics::PlaySound2D(this, HoverSound);
	}
}
