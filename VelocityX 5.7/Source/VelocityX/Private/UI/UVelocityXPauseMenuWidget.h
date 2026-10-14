// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UVelocityXPauseMenuWidget.generated.h"

class UButton;
class UTextBlock;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVelocityXPauseResumeClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVelocityXPauseRestartClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVelocityXPauseSettingsClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVelocityXPauseQuitToMenuClicked);

/**
 * UVelocityXPauseMenuWidget
 * C++ Controller for VELOCITY X In-Race Pause Modal Overlay.
 * Manages game pausing/unpausing, restart triggers, settings modal calls,
 * and level exit transitions.
 */
UCLASS()
class VELOCITYX_API UVelocityXPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UVelocityXPauseMenuWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// --- Blueprint Assignable Delegates ---
	UPROPERTY(BlueprintAssignable, Category = "VelocityX|Pause|Events")
	FOnVelocityXPauseResumeClicked OnResumeClicked;

	UPROPERTY(BlueprintAssignable, Category = "VelocityX|Pause|Events")
	FOnVelocityXPauseRestartClicked OnRestartClicked;

	UPROPERTY(BlueprintAssignable, Category = "VelocityX|Pause|Events")
	FOnVelocityXPauseSettingsClicked OnSettingsClicked;

	UPROPERTY(BlueprintAssignable, Category = "VelocityX|Pause|Events")
	FOnVelocityXPauseQuitToMenuClicked OnQuitToMenuClicked;

	// --- Public API ---
	UFUNCTION(BlueprintCallable, Category = "VelocityX|Pause")
	void SetRaceTelemetry(float CurrentRaceSeconds, const FString& VehicleName);

	UFUNCTION(BlueprintCallable, Category = "VelocityX|Pause")
	void ResumeGame();

protected:
	// --- UMG Bound Widgets: Buttons ---
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "VelocityX|Pause|Widgets")
	TObjectPtr<UButton> ResumeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "VelocityX|Pause|Widgets")
	TObjectPtr<UButton> RestartButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "VelocityX|Pause|Widgets")
	TObjectPtr<UButton> SettingsButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "VelocityX|Pause|Widgets")
	TObjectPtr<UButton> QuitToMenuButton;

	// --- UMG Bound Widgets: Info ---
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|Pause|Widgets")
	TObjectPtr<UTextBlock> CurrentTimeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|Pause|Widgets")
	TObjectPtr<UTextBlock> VehicleModelText;

	// --- Audio Effects ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VelocityX|Pause|Audio")
	TObjectPtr<USoundBase> PauseOpenSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VelocityX|Pause|Audio")
	TObjectPtr<USoundBase> PauseResumeSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VelocityX|Pause|Audio")
	TObjectPtr<USoundBase> ClickSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VelocityX|Pause|Audio")
	TObjectPtr<USoundBase> HoverSound;

	// Internal handlers
	UFUNCTION()
	void HandleResumeClicked();

	UFUNCTION()
	void HandleRestartClicked();

	UFUNCTION()
	void HandleSettingsClicked();

	UFUNCTION()
	void HandleQuitToMenuClicked();

	UFUNCTION()
	void HandleHover();
};
