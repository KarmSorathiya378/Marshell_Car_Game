// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VELOCITYXCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

/**
 * VELOCITY X Base Character
 * Standard human player character for paddock, garage, or free-roam scenes.
 */
UCLASS()
class VELOCITYX_API AVELOCITYXCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AVELOCITYXCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// --- Movement Callbacks ---
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveForward(float Value);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveRight(float Value);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void TurnAtRate(float Rate);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void LookUpAtRate(float Rate);

	// --- Vehicle Interaction ---
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryEnterVehicle();

	// --- Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseLookUpRate;
};
