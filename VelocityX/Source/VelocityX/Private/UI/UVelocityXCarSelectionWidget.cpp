// Copyright VELOCITY X Racing. All Rights Reserved.

#include "UVelocityXCarSelectionWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

UVelocityXCarSelectionWidget::UVelocityXCarSelectionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InitializeDefaultCarSpecs();
}

void UVelocityXCarSelectionWidget::InitializeDefaultCarSpecs()
{
	CarSpecs.Empty();

	// 0: Supercar (Apex GT-1)
	FVelocityXCarCardData Supercar;
	Supercar.ModelName = TEXT("APEX GT-1");
	Supercar.ClassTag = TEXT("SUPERCAR // BALANCED GT");
	Supercar.TopSpeedKmh = 320.0f;
	Supercar.AccelerationSeconds = 3.5f;
	Supercar.HandlingScore = 85.0f;
	Supercar.Horsepower = 500;
	Supercar.TorqueNm = 470;
	Supercar.SpecialPerk = TEXT("BALANCED SLIP ANGLE CONTROL");
	Supercar.BestLapRecord = TEXT("--:--.---");
	CarSpecs.Add(Supercar);

	// 1: Track Supercar (Valkyrie RS)
	FVelocityXCarCardData TrackSupercar;
	TrackSupercar.ModelName = TEXT("VALKYRIE RS");
	TrackSupercar.ClassTag = TEXT("TRACK SUPERCAR // DOWNFORCE & DRS");
	TrackSupercar.TopSpeedKmh = 355.0f;
	TrackSupercar.AccelerationSeconds = 2.8f;
	TrackSupercar.HandlingScore = 98.0f;
	TrackSupercar.Horsepower = 720;
	TrackSupercar.TorqueNm = 800;
	TrackSupercar.SpecialPerk = TEXT("ACTIVE DUAL-ELEMENT WING & DRS");
	TrackSupercar.BestLapRecord = TEXT("--:--.---");
	CarSpecs.Add(TrackSupercar);

	// 2: Hypercar (Nebula Hyper-V)
	FVelocityXCarCardData Hypercar;
	Hypercar.ModelName = TEXT("NEBULA HYPER-V");
	Hypercar.ClassTag = TEXT("HYPERCAR // QUAD-TURBO AWD");
	Hypercar.TopSpeedKmh = 420.0f;
	Hypercar.AccelerationSeconds = 2.4f;
	Hypercar.HandlingScore = 78.0f;
	Hypercar.Horsepower = 1500;
	Hypercar.TorqueNm = 1600;
	Hypercar.SpecialPerk = TEXT("QUAD-TURBO HYBRID BOOST & CATAPULT AWD");
	Hypercar.BestLapRecord = TEXT("--:--.---");
	CarSpecs.Add(Hypercar);
}

void UVelocityXCarSelectionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Bind Thumbnail Buttons
	if (ThumbnailButton0)
	{
		ThumbnailButton0->OnClicked.AddDynamic(this, &UVelocityXCarSelectionWidget::HandleThumbnail0Clicked);
		ThumbnailButton0->OnHovered.AddDynamic(this, &UVelocityXCarSelectionWidget::HandleHover);
	}

	if (ThumbnailButton1)
	{
		ThumbnailButton1->OnClicked.AddDynamic(this, &UVelocityXCarSelectionWidget::HandleThumbnail1Clicked);
		ThumbnailButton1->OnHovered.AddDynamic(this, &UVelocityXCarSelectionWidget::HandleHover);
	}

	if (ThumbnailButton2)
	{
		ThumbnailButton2->OnClicked.AddDynamic(this, &UVelocityXCarSelectionWidget::HandleThumbnail2Clicked);
		ThumbnailButton2->OnHovered.AddDynamic(this, &UVelocityXCarSelectionWidget::HandleHover);
	}

	// Bind Confirm Button
	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UVelocityXCarSelectionWidget::HandleConfirmClicked);
		ConfirmButton->OnHovered.AddDynamic(this, &UVelocityXCarSelectionWidget::HandleHover);
	}

	// Bind Back Button
	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UVelocityXCarSelectionWidget::HandleBackClicked);
		BackButton->OnHovered.AddDynamic(this, &UVelocityXCarSelectionWidget::HandleHover);
	}
}

void UVelocityXCarSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SelectCarByIndex(0);
}

void UVelocityXCarSelectionWidget::SelectCarByIndex(int32 NewIndex)
{
	if (!CarSpecs.IsValidIndex(NewIndex))
	{
		return;
	}

	CurrentCarIndex = NewIndex;

	if (SelectSound)
	{
		UGameplayStatics::PlaySound2D(this, SelectSound);
	}

	RefreshUIForCurrentCar();

	OnCarIndexChanged.Broadcast(CurrentCarIndex);
}

FVelocityXCarCardData UVelocityXCarSelectionWidget::GetCurrentCarData() const
{
	if (CarSpecs.IsValidIndex(CurrentCarIndex))
	{
		return CarSpecs[CurrentCarIndex];
	}
	return FVelocityXCarCardData();
}

void UVelocityXCarSelectionWidget::UpdateCarLapRecord(int32 CarIndex, const FString& LapRecordStr)
{
	if (CarSpecs.IsValidIndex(CarIndex))
	{
		CarSpecs[CarIndex].BestLapRecord = LapRecordStr;
		if (CarIndex == CurrentCarIndex && RecordTimeText)
		{
			RecordTimeText->SetText(FText::Format(NSLOCTEXT("VelocityX", "LapRec", "RECORD: {0}"), FText::FromString(LapRecordStr)));
		}
	}
}

void UVelocityXCarSelectionWidget::RefreshUIForCurrentCar()
{
	const FVelocityXCarCardData& Spec = CarSpecs[CurrentCarIndex];

	if (CarModelText)
	{
		CarModelText->SetText(FText::FromString(Spec.ModelName));
	}

	if (CarClassText)
	{
		CarClassText->SetText(FText::FromString(Spec.ClassTag));
	}

	if (HorsepowerText)
	{
		FString HPStr = FString::Printf(TEXT("%d HP (%d N*m)"), Spec.Horsepower, Spec.TorqueNm);
		HorsepowerText->SetText(FText::FromString(HPStr));
	}

	if (TopSpeedText)
	{
		float Mph = Spec.TopSpeedKmh * 0.621371f;
		FString SpeedStr = FString::Printf(TEXT("%.0f KM/H (%.0f MPH)"), Spec.TopSpeedKmh, Mph);
		TopSpeedText->SetText(FText::FromString(SpeedStr));
	}

	if (AccelerationText)
	{
		FString AccStr = FString::Printf(TEXT("%.1f SECONDS"), Spec.AccelerationSeconds);
		AccelerationText->SetText(FText::FromString(AccStr));
	}

	if (HandlingText)
	{
		FString HandStr = FString::Printf(TEXT("%.0f / 100"), Spec.HandlingScore);
		HandlingText->SetText(FText::FromString(HandStr));
	}

	if (SpecialPerkText)
	{
		SpecialPerkText->SetText(FText::Format(NSLOCTEXT("VelocityX", "SpecialFormat", "SPECIAL: {0}"), FText::FromString(Spec.SpecialPerk)));
	}

	if (RecordTimeText)
	{
		RecordTimeText->SetText(FText::Format(NSLOCTEXT("VelocityX", "RecordFormat", "RECORD: {0}"), FText::FromString(Spec.BestLapRecord)));
	}

	// Normalized Progress Bars:
	// Top speed: 300 - 450 km/h -> map to 0.6 - 1.0
	if (TopSpeedBar)
	{
		TopSpeedBar->SetPercent(FMath::Clamp(Spec.TopSpeedKmh / 450.0f, 0.0f, 1.0f));
	}

	// Acceleration: 2.0s = 1.0, 4.0s = 0.5
	if (AccelerationBar)
	{
		AccelerationBar->SetPercent(FMath::Clamp(4.0f / (Spec.AccelerationSeconds * 1.05f), 0.0f, 1.0f));
	}

	// Handling: 0 - 100
	if (HandlingBar)
	{
		HandlingBar->SetPercent(FMath::Clamp(Spec.HandlingScore / 100.0f, 0.0f, 1.0f));
	}
}

void UVelocityXCarSelectionWidget::HandleThumbnail0Clicked()
{
	SelectCarByIndex(0);
}

void UVelocityXCarSelectionWidget::HandleThumbnail1Clicked()
{
	SelectCarByIndex(1);
}

void UVelocityXCarSelectionWidget::HandleThumbnail2Clicked()
{
	SelectCarByIndex(2);
}

void UVelocityXCarSelectionWidget::HandleConfirmClicked()
{
	if (ConfirmSound)
	{
		UGameplayStatics::PlaySound2D(this, ConfirmSound);
	}

	UE_LOG(LogTemp, Display, TEXT("[VelocityX UI] Car Selection Confirmed: Index %d (%s)"), 
		CurrentCarIndex, *CarSpecs[CurrentCarIndex].ModelName);

	OnCarConfirmed.Broadcast(CurrentCarIndex);
}

void UVelocityXCarSelectionWidget::HandleBackClicked()
{
	if (SelectSound)
	{
		UGameplayStatics::PlaySound2D(this, SelectSound);
	}

	OnBackClicked.Broadcast();
}

void UVelocityXCarSelectionWidget::HandleHover()
{
	if (HoverSound)
	{
		UGameplayStatics::PlaySound2D(this, HoverSound);
	}
}
