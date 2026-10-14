// Copyright VELOCITY X Racing. All Rights Reserved.

#include "UVelocityXHUDWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

const FLinearColor UVelocityXHUDWidget::NeonCyan(0.0f, 0.9f, 1.0f, 1.0f);
const FLinearColor UVelocityXHUDWidget::NeonGreen(0.0f, 1.0f, 0.4f, 1.0f);
const FLinearColor UVelocityXHUDWidget::VelocityCrimson(1.0f, 0.1f, 0.2f, 1.0f);
const FLinearColor UVelocityXHUDWidget::MutedGray(0.4f, 0.5f, 0.6f, 0.7f);

UVelocityXHUDWidget::UVelocityXHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bHasScriptImplementedTick = true;
}

void UVelocityXHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (PauseButton)
	{
		PauseButton->OnClicked.AddDynamic(this, &UVelocityXHUDWidget::HandlePauseClicked);
	}
}

void UVelocityXHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Initialize default values
	SetSpeed(0.0f, false);
	SetRPM(900.0f, 9000.0f);
	SetGear(1);
	SetRaceTime(0.0f);
	SetHeadlights(false);
	SetWarningBanner(TEXT(""), false);
}

void UVelocityXHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Redline Flashing Animation
	if (bIsRedlineActive && RedlineWarningIcon)
	{
		RedlineFlashTimer += InDeltaTime * 12.0f; // 12 Hz strobe
		float Alpha = (FMath::Sin(RedlineFlashTimer) > 0.0f) ? 1.0f : 0.2f;
		RedlineWarningIcon->SetRenderOpacity(Alpha);
	}
}

void UVelocityXHUDWidget::SetSpeed(float SpeedKPH, bool bDisplayMPH)
{
	float FinalSpeed = bDisplayMPH ? (SpeedKPH * 0.621371f) : SpeedKPH;
	int32 IntSpeed = FMath::Max(0, FMath::RoundToInt(FinalSpeed));

	if (SpeedText)
	{
		SpeedText->SetText(FText::AsNumber(IntSpeed));
	}

	if (SpeedUnitText)
	{
		SpeedUnitText->SetText(bDisplayMPH ? 
			NSLOCTEXT("VelocityX", "MPH", "MPH") : 
			NSLOCTEXT("VelocityX", "KPH", "KM/H"));
	}

	if (SpeedProgressBar)
	{
		// Map 0 - 420 km/h to 0.0 - 1.0
		float NormalizedSpeed = FMath::Clamp(SpeedKPH / 420.0f, 0.0f, 1.0f);
		SpeedProgressBar->SetPercent(NormalizedSpeed);

		// Dynamic color shift based on velocity
		FLinearColor SpeedColor = (NormalizedSpeed < 0.6f) 
			? FLinearColor::LerpUsingHSV(NeonCyan, NeonGreen, NormalizedSpeed / 0.6f)
			: FLinearColor::LerpUsingHSV(NeonGreen, VelocityCrimson, (NormalizedSpeed - 0.6f) / 0.4f);
		SpeedProgressBar->SetFillColorAndOpacity(SpeedColor);
	}
}

void UVelocityXHUDWidget::SetRPM(float CurrentRPM, float RedlineRPM)
{
	float RPMRatio = FMath::Clamp(CurrentRPM / RedlineRPM, 0.0f, 1.0f);

	if (RPMProgressBar)
	{
		RPMProgressBar->SetPercent(RPMRatio);

		if (RPMRatio >= 0.88f)
		{
			RPMProgressBar->SetFillColorAndOpacity(VelocityCrimson);
		}
		else if (RPMRatio >= 0.65f)
		{
			RPMProgressBar->SetFillColorAndOpacity(FLinearColor(1.0f, 0.8f, 0.0f, 1.0f)); // Amber
		}
		else
		{
			RPMProgressBar->SetFillColorAndOpacity(NeonCyan);
		}
	}

	if (RPMText)
	{
		RPMText->SetText(FText::AsNumber(FMath::RoundToInt(CurrentRPM)));
	}

	// Redline Warning Evaluation
	bool bShouldRedline = (CurrentRPM >= RedlineRPM * 0.92f);
	if (bShouldRedline != bIsRedlineActive)
	{
		bIsRedlineActive = bShouldRedline;
		if (bIsRedlineActive)
		{
			if (RedlineBeepSound)
			{
				UGameplayStatics::PlaySound2D(this, RedlineBeepSound);
			}
			if (RedlineWarningIcon)
			{
				RedlineWarningIcon->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			if (RedlineWarningIcon)
			{
				RedlineWarningIcon->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void UVelocityXHUDWidget::SetGear(int32 Gear)
{
	if (Gear != CachedGear)
	{
		if (GearShiftSound)
		{
			UGameplayStatics::PlaySound2D(this, GearShiftSound);
		}
		CachedGear = Gear;
	}

	if (GearText)
	{
		FString GearStr;
		switch (Gear)
		{
		case -1:
			GearStr = TEXT("R");
			break;
		case 0:
			GearStr = TEXT("N");
			break;
		default:
			GearStr = FString::FromInt(Gear);
			break;
		}
		GearText->SetText(FText::FromString(GearStr));
	}
}

void UVelocityXHUDWidget::SetRaceTime(float ElapsedSeconds)
{
	if (TimerText)
	{
		int32 Minutes = FMath::FloorToInt(ElapsedSeconds / 60.0f);
		int32 Seconds = FMath::FloorToInt(FMath::Fmod(ElapsedSeconds, 60.0f));
		int32 Millis  = FMath::FloorToInt(FMath::Frac(ElapsedSeconds) * 1000.0f);

		FString TimeString = FString::Printf(TEXT("%02d:%02d.%03d"), Minutes, Seconds, Millis);
		TimerText->SetText(FText::FromString(TimeString));
	}
}

void UVelocityXHUDWidget::SetHeadlights(bool bActive)
{
	if (HeadlightIcon)
	{
		HeadlightIcon->SetColorAndOpacity(bActive ? NeonGreen : MutedGray);
	}

	if (HeadlightText)
	{
		HeadlightText->SetText(bActive ? 
			NSLOCTEXT("VelocityX", "BeamOn", "BEAM: ON") : 
			NSLOCTEXT("VelocityX", "BeamOff", "BEAM: OFF"));
		HeadlightText->SetColorAndOpacity(FSlateColor(bActive ? NeonGreen : MutedGray));
	}
}

void UVelocityXHUDWidget::SetWarningBanner(const FString& WarningMessage, bool bShow)
{
	if (WarningBannerText)
	{
		if (bShow && !WarningMessage.IsEmpty())
		{
			WarningBannerText->SetText(FText::FromString(WarningMessage));
			WarningBannerText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			WarningBannerText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UVelocityXHUDWidget::SetVehicleModelName(const FString& ModelName)
{
	if (VehicleModelText)
	{
		VehicleModelText->SetText(FText::FromString(ModelName));
	}
}

void UVelocityXHUDWidget::HandlePauseClicked()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound);
	}

	UE_LOG(LogTemp, Display, TEXT("[VelocityX UI] HUD Pause Button Clicked."));
	OnPauseButtonClicked.Broadcast();
}
