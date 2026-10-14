// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UVelocityXHUDWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;
class UProgressBar;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVelocityXPauseButtonClicked);

/**
 * UVelocityXHUDWidget
 * High-performance C++ In-Game HUD Controller.
 * Provides real-time telemetry updates for Speedometer, Tachometer (RPM),
 * Gear readout, Stopwatch, Headlights, and Pause trigger.
 */
UCLASS()
class VELOCITYX_API UVelocityXHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UVelocityXHUDWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// --- Blueprint Assignable Delegates ---
	UPROPERTY(BlueprintAssignable, Category = "VelocityX|HUD|Events")
	FOnVelocityXPauseButtonClicked OnPauseButtonClicked;

	// --- High-Performance Public Update API ---

	/** Update speedometer with speed in km/h. bDisplayMPH converts unit to MPH. */
	UFUNCTION(BlueprintCallable, Category = "VelocityX|HUD")
	void SetSpeed(float SpeedKPH, bool bDisplayMPH = false);

	/** Update RPM bar and evaluate redline warning */
	UFUNCTION(BlueprintCallable, Category = "VelocityX|HUD")
	void SetRPM(float CurrentRPM, float RedlineRPM = 9000.0f);

	/** Update centered gear indicator (-1 = Reverse, 0 = Neutral, 1..7 = Gears) */
	UFUNCTION(BlueprintCallable, Category = "VelocityX|HUD")
	void SetGear(int32 Gear);

	/** Update elapsed race timer in seconds */
	UFUNCTION(BlueprintCallable, Category = "VelocityX|HUD")
	void SetRaceTime(float ElapsedSeconds);

	/** Update headlight active icon state */
	UFUNCTION(BlueprintCallable, Category = "VelocityX|HUD")
	void SetHeadlights(bool bActive);

	/** Trigger hazard/safety warning banner */
	UFUNCTION(BlueprintCallable, Category = "VelocityX|HUD")
	void SetWarningBanner(const FString& WarningMessage, bool bShow);

	/** Set player vehicle name badge */
	UFUNCTION(BlueprintCallable, Category = "VelocityX|HUD")
	void SetVehicleModelName(const FString& ModelName);

protected:
	// --- UMG Bound Widgets: Speedometer Cluster ---
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "VelocityX|HUD|Widgets")
	TObjectPtr<UTextBlock> SpeedText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|HUD|Widgets")
	TObjectPtr<UTextBlock> SpeedUnitText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|HUD|Widgets")
	TObjectPtr<UProgressBar> SpeedProgressBar;

	// --- UMG Bound Widgets: RPM & Tachometer ---
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|HUD|Widgets")
	TObjectPtr<UProgressBar> RPMProgressBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|HUD|Widgets")
	TObjectPtr<UTextBlock> RPMText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|HUD|Widgets")
	TObjectPtr<UImage> RedlineWarningIcon;

	// --- UMG Bound Widgets: Gear ---
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "VelocityX|HUD|Widgets")
	TObjectPtr<UTextBlock> GearText;

	// --- UMG Bound Widgets: Stopwatch Timer ---
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "VelocityX|HUD|Widgets")
	TObjectPtr<UTextBlock> TimerText;

	// --- UMG Bound Widgets: Headlights & Status ---
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|HUD|Widgets")
	TObjectPtr<UImage> HeadlightIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|HUD|Widgets")
	TObjectPtr<UTextBlock> HeadlightText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|HUD|Widgets")
	TObjectPtr<UTextBlock> VehicleModelText;

	// --- UMG Bound Widgets: Interactive Pause Button ---
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "VelocityX|HUD|Widgets")
	TObjectPtr<UButton> PauseButton;

	// --- UMG Bound Widgets: Warning Banner ---
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|HUD|Widgets")
	TObjectPtr<UTextBlock> WarningBannerText;

	// --- Audio Effects ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VelocityX|HUD|Audio")
	TObjectPtr<USoundBase> GearShiftSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VelocityX|HUD|Audio")
	TObjectPtr<USoundBase> RedlineBeepSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VelocityX|HUD|Audio")
	TObjectPtr<USoundBase> ClickSound;

	// State trackers
	int32 CachedGear = 1;
	bool bIsRedlineActive = false;
	float RedlineFlashTimer = 0.0f;

	UFUNCTION()
	void HandlePauseClicked();

	// Color palette
	static const FLinearColor NeonCyan;
	static const FLinearColor NeonGreen;
	static const FLinearColor VelocityCrimson;
	static const FLinearColor MutedGray;
};
