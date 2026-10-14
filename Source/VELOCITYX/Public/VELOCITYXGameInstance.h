// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameInstance/VelocityGameInstance.h"
#include "VELOCITYXGameMode.h"
#include "VELOCITYXGameInstance.generated.h"

/**
 * Graphics quality presets
 */
UENUM(BlueprintType)
enum class EGraphicsQuality : uint8
{
	Low    UMETA(DisplayName = "Low"),
	Medium UMETA(DisplayName = "Medium"),
	High   UMETA(DisplayName = "High"),
	Ultra  UMETA(DisplayName = "Ultra")
};

/**
 * Save data structure for persistent game profile
 */
USTRUCT(BlueprintType)
struct FVelocitySaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveData")
	int32 SelectedCarIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveData")
	float TotalPlayTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveData")
	int32 CompletedRaces = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveData")
	TArray<float> BestLapTimes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveData")
	float MasterVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveData")
	float SFXVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveData")
	float MusicVolume = 0.8f;
};

/**
 * VELOCITY X Game Instance
 * Extends UVelocityGameInstance with car selection, graphic presets, audio levels, and global profile persistence.
 * Primary C++ class for BP_GameInstance.
 */
UCLASS(Blueprintable, BlueprintType)
class VELOCITYX_API UVELOCITYXGameInstance : public UVelocityGameInstance
{
	GENERATED_BODY()

public:
	UVELOCITYXGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

	// --- Save / Load Data ---
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool SaveGameData();

	UFUNCTION(BlueprintCallable, Category = "Save")
	bool LoadGameData();

	// --- Car Selection State ---
	UFUNCTION(BlueprintCallable, Category = "Car Selection")
	void SetSelectedCar(ECarType NewCarType);

	UFUNCTION(BlueprintPure, Category = "Car Selection")
	ECarType GetSelectedCar() const { return ActiveCarType; }

	// --- Settings ---
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplyDefaultSettings();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	EGraphicsQuality GraphicsQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 ShadowResolution;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int32 TextureQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	bool bMotionBlur;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	bool bAntiAliasing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	float FieldOfView;

	// --- Audio Settings ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float MasterVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float SFXVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float MusicVolume;

	// --- Game Progress ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	float TotalPlayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	int32 CompletedRaces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	TArray<float> BestLapTimes;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Car Selection")
	ECarType ActiveCarType;

	FString GetSaveFilePath() const;
};
