// Copyright VELOCITY X Racing. All Rights Reserved.

#include "VelocityXUIConfig.h"
#include "Blueprint/UserWidget.h"

UVelocityXUIConfig::UVelocityXUIConfig()
{
	MainMenuWidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(TEXT("/Game/Blueprints/UI/BP_MainMenu.BP_MainMenu_C")));
	CarSelectionWidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(TEXT("/Game/Blueprints/UI/BP_CarSelection.BP_CarSelection_C")));
	HUDWidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(TEXT("/Game/Blueprints/UI/BP_VelocityHUD.BP_VelocityHUD_C")));
	PauseMenuWidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(TEXT("/Game/Blueprints/UI/BP_PauseMenu.BP_PauseMenu_C")));
	SettingsWidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(TEXT("/Game/Blueprints/UI/BP_Settings.BP_Settings_C")));
}

const UVelocityXUIConfig* UVelocityXUIConfig::GetUIConfig()
{
	return GetDefault<UVelocityXUIConfig>();
}
