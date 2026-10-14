// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VelocityXUIConfig.generated.h"

class UUserWidget;

/**
 * VelocityX UI Configuration & Widget Class Registry
 * Reads from DefaultGame.ini [/Script/VelocityX.VelocityXUIConfig]
 */
UCLASS(Config = Game, DefaultConfig)
class VELOCITYX_API UVelocityXUIConfig : public UObject
{
	GENERATED_BODY()

public:
	UVelocityXUIConfig();

	/** Main Menu Widget Blueprint class reference */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "VelocityX|UI")
	TSoftClassPtr<UUserWidget> MainMenuWidgetClass;

	/** Car Selection Widget Blueprint class reference */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "VelocityX|UI")
	TSoftClassPtr<UUserWidget> CarSelectionWidgetClass;

	/** In-Game Telemetry HUD Widget Blueprint class reference */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "VelocityX|UI")
	TSoftClassPtr<UUserWidget> HUDWidgetClass;

	/** Pause Menu Modal Widget Blueprint class reference */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "VelocityX|UI")
	TSoftClassPtr<UUserWidget> PauseMenuWidgetClass;

	/** Settings Menu Modal Widget Blueprint class reference */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "VelocityX|UI")
	TSoftClassPtr<UUserWidget> SettingsWidgetClass;

	/** Static helper to get UI configuration singleton */
	UFUNCTION(BlueprintPure, Category = "VelocityX|UI")
	static const UVelocityXUIConfig* GetUIConfig();
};
