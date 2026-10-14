// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "VELOCITYXMainMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMainMenuButtonClicked);

/**
 * VELOCITY X Main Menu User Widget
 */
UCLASS()
class VELOCITYX_API UVELOCITYXMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeOnClosed();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMainMenuButtonClicked OnPlayButtonClickedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMainMenuButtonClicked OnSettingsButtonClickedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMainMenuButtonClicked OnQuitButtonClickedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMainMenuButtonClicked OnCreditsButtonClickedDelegate;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UButton* PlayButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* SettingsButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* QuitButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* CreditsButton;

	UFUNCTION()
	void OnPlayButtonClicked();

	UFUNCTION()
	void OnSettingsButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();

	UFUNCTION()
	void OnCreditsButtonClicked();
};
