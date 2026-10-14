// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VELOCITYXGameMode.h" // For ECarType
#include "CarManager.generated.h"

class ABaseCar;
class ASupercar;
class ATrackSupercar;
class AHypercar;
class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCarSpawnedSignature, ECarType, CarType, ABaseCar*, SpawnedCar);

/**
 * Car Manager
 * Coordinates vehicle spawning, class resolution, and active player vehicle assignment.
 */
UCLASS()
class VELOCITYX_API ACarManager : public AActor
{
	GENERATED_BODY()

public:
	ACarManager();

	virtual void BeginPlay() override;

	// --- Vehicle Spawning & Selection ---
	UFUNCTION(BlueprintCallable, Category = "Car Manager")
	ABaseCar* SpawnCar(ECarType CarType, const FTransform& SpawnTransform);

	UFUNCTION(BlueprintCallable, Category = "Car Manager")
	ABaseCar* SpawnAndPossessCar(ECarType CarType, const FTransform& SpawnTransform, APlayerController* PC);

	UFUNCTION(BlueprintCallable, Category = "Car Manager")
	void PossessCar(APlayerController* PC, ABaseCar* TargetCar);

	UFUNCTION(BlueprintPure, Category = "Car Manager")
	TSubclassOf<ABaseCar> GetCarClassForType(ECarType CarType) const;

	UFUNCTION(BlueprintPure, Category = "Car Manager")
	ABaseCar* GetActivePlayerCar() const { return ActivePlayerCar; }

	// --- Vehicle Blueprints / Classes ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Classes")
	TSubclassOf<ABaseCar> SupercarClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Classes")
	TSubclassOf<ABaseCar> TrackSupercarClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car Classes")
	TSubclassOf<ABaseCar> HypercarClass;

	// --- Events ---
	UPROPERTY(BlueprintAssignable, Category = "Car Manager")
	FOnCarSpawnedSignature OnCarSpawned;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Car Manager")
	ABaseCar* ActivePlayerCar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Car Manager")
	TArray<ABaseCar*> SpawnedFleet;
};
