// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UVelocityXMainMenuWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMainMenuPlayClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMainMenuCarSelectClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMainMenuSettingsClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMainMenuQuitClicked);

/**
 * UVelocityXMainMenuWidget
 * C++ controller for VELOCITY X Main Menu.
 * Implements a modern dark/neon aesthetic with cyber-cyan and velocity-crimson accents.
 */
UCLASS()
class VELOCITYX_API UVelocityXMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UVelocityXMainMenuWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// --- Event Delegates for Blueprints ---
	UPROPERTY(BlueprintAssignable, Category = "VelocityX|UI|Events")
	FOnMainMenuPlayClicked OnPlayClicked;

	UPROPERTY(BlueprintAssignable, Category = "VelocityX|UI|Events")
	FOnMainMenuCarSelectClicked OnCarSelectClicked;

	UPROPERTY(BlueprintAssignable, Category = "VelocityX|UI|Events")
	FOnMainMenuSettingsClicked OnSettingsClicked;

	UPROPERTY(BlueprintAssignable, Category = "VelocityX|UI|Events")
	FOnMainMenuQuitClicked OnQuitClicked;

	// --- Public Styling & UI API ---
	UFUNCTION(BlueprintCallable, Category = "VelocityX|UI")
	void SetCareerStats(int32 CompletedRaces, float TotalPlayTimeSeconds, const FString& BestLapTimeStr);

	UFUNCTION(BlueprintCallable, Category = "VelocityX|UI")
	void ApplyNeonStyling();

protected:
	// --- UMG Bound Widgets (BindWidget) ---
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Widgets")
	TObjectPtr<UImage> TitleLogo;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Widgets")
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Widgets")
	TObjectPtr<UTextBlock> SubtitleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "VelocityX|UI|Widgets")
	TObjectPtr<UButton> PlayButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Widgets")
	TObjectPtr<UButton> CarSelectButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "VelocityX|UI|Widgets")
	TObjectPtr<UButton> SettingsButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "VelocityX|UI|Widgets")
	TObjectPtr<UButton> QuitButton;

	// Profile & Stats Labels
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Widgets")
	TObjectPtr<UTextBlock> CompletedRacesText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Widgets")
	TObjectPtr<UTextBlock> PlayTimeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Widgets")
	TObjectPtr<UTextBlock> BestRecordText;

	// --- Audio Sound Effects ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VelocityX|UI|Audio")
	TObjectPtr<USoundBase> HoverSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VelocityX|UI|Audio")
	TObjectPtr<USoundBase> ClickSound;

	// --- Button Click & Hover Handlers ---
	UFUNCTION()
	void HandlePlayClicked();

	UFUNCTION()
	void HandleCarSelectClicked();

	UFUNCTION()
	void HandleSettingsClicked();

	UFUNCTION()
	void HandleQuitClicked();

	UFUNCTION()
	void HandleGenericHover();

	// Color Constants for Modern Dark/Neon Palette
	static const FLinearColor ColorObsidianBlack;
	static const FLinearColor ColorNeonCyan;
	static const FLinearColor ColorVelocityCrimson;
	static const FLinearColor ColorPlatinumWhite;
};
