// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UVelocityXCarSelectionWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;
class UProgressBar;
class USoundBase;

/**
 * Performance specifications and profile for vehicle selection cards.
 */
USTRUCT(BlueprintType)
struct FVelocityXCarCardData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|CarData")
	FString ModelName = TEXT("APEX GT-1");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|CarData")
	FString ClassTag = TEXT("SUPERCAR // BALANCED GT");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|CarData")
	float TopSpeedKmh = 320.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|CarData")
	float AccelerationSeconds = 3.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|CarData")
	float HandlingScore = 85.0f; // 0 - 100

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|CarData")
	int32 Horsepower = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|CarData")
	int32 TorqueNm = 470;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|CarData")
	FString SpecialPerk = TEXT("BALANCED SLIP ANGLE CONTROL");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VelocityX|CarData")
	FString BestLapRecord = TEXT("--:--.---");
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVelocityXCarIndexChanged, int32, NewCarIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVelocityXCarConfirmed, int32, ConfirmedCarIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVelocityXCarSelectBack);

/**
 * UVelocityXCarSelectionWidget
 * C++ controller for Car Selection Screen displaying 3 selectable vehicle classes:
 * 0: Supercar (Apex GT-1)
 * 1: Track Supercar (Valkyrie RS)
 * 2: Hypercar (Nebula Hyper-V)
 */
UCLASS()
class VELOCITYX_API UVelocityXCarSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UVelocityXCarSelectionWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	// --- Blueprint Assignable Delegates ---
	UPROPERTY(BlueprintAssignable, Category = "VelocityX|UI|Events")
	FOnVelocityXCarIndexChanged OnCarIndexChanged;

	UPROPERTY(BlueprintAssignable, Category = "VelocityX|UI|Events")
	FOnVelocityXCarConfirmed OnCarConfirmed;

	UPROPERTY(BlueprintAssignable, Category = "VelocityX|UI|Events")
	FOnVelocityXCarSelectBack OnBackClicked;

	// --- Public API ---
	UFUNCTION(BlueprintCallable, Category = "VelocityX|UI")
	void SelectCarByIndex(int32 NewIndex);

	UFUNCTION(BlueprintPure, Category = "VelocityX|UI")
	int32 GetSelectedCarIndex() const { return CurrentCarIndex; }

	UFUNCTION(BlueprintPure, Category = "VelocityX|UI")
	FVelocityXCarCardData GetCurrentCarData() const;

	UFUNCTION(BlueprintCallable, Category = "VelocityX|UI")
	void UpdateCarLapRecord(int32 CarIndex, const FString& LapRecordStr);

protected:
	// --- UMG Bound Widgets: 3 Selectable Thumbnails ---
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "VelocityX|UI|Thumbnails")
	TObjectPtr<UButton> ThumbnailButton0; // Supercar

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "VelocityX|UI|Thumbnails")
	TObjectPtr<UButton> ThumbnailButton1; // TrackSupercar

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "VelocityX|UI|Thumbnails")
	TObjectPtr<UButton> ThumbnailButton2; // Hypercar

	// Active Card Details
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Stats")
	TObjectPtr<UTextBlock> CarModelText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Stats")
	TObjectPtr<UTextBlock> CarClassText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Stats")
	TObjectPtr<UTextBlock> HorsepowerText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Stats")
	TObjectPtr<UTextBlock> TopSpeedText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Stats")
	TObjectPtr<UTextBlock> AccelerationText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Stats")
	TObjectPtr<UTextBlock> HandlingText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Stats")
	TObjectPtr<UTextBlock> SpecialPerkText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Stats")
	TObjectPtr<UTextBlock> RecordTimeText;

	// Stat Progress Bars
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Bars")
	TObjectPtr<UProgressBar> TopSpeedBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Bars")
	TObjectPtr<UProgressBar> AccelerationBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Bars")
	TObjectPtr<UProgressBar> HandlingBar;

	// Actions
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "VelocityX|UI|Actions")
	TObjectPtr<UButton> ConfirmButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "VelocityX|UI|Actions")
	TObjectPtr<UButton> BackButton;

	// --- Audio Effects ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VelocityX|UI|Audio")
	TObjectPtr<USoundBase> SelectSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VelocityX|UI|Audio")
	TObjectPtr<USoundBase> ConfirmSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VelocityX|UI|Audio")
	TObjectPtr<USoundBase> HoverSound;

	// --- Internal Data ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VelocityX|CarData")
	TArray<FVelocityXCarCardData> CarSpecs;

	UPROPERTY(BlueprintReadOnly, Category = "VelocityX|CarData")
	int32 CurrentCarIndex = 0;

	// Internal handlers
	UFUNCTION()
	void HandleThumbnail0Clicked();

	UFUNCTION()
	void HandleThumbnail1Clicked();

	UFUNCTION()
	void HandleThumbnail2Clicked();

	UFUNCTION()
	void HandleConfirmClicked();

	UFUNCTION()
	void HandleBackClicked();

	UFUNCTION()
	void HandleHover();

	void RefreshUIForCurrentCar();
	void InitializeDefaultCarSpecs();
};
