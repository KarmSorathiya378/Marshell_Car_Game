// Copyright VELOCITY X Racing. All Rights Reserved.

#include "Vehicle/CarManager.h"
#include "Vehicle/BaseCar.h"
#include "Vehicle/Supercar.h"
#include "Vehicle/TrackSupercar.h"
#include "Vehicle/Hypercar.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

ACarManager::ACarManager()
{
	PrimaryActorTick.bCanEverTick = false;

	SupercarClass = ASupercar::StaticClass();
	TrackSupercarClass = ATrackSupercar::StaticClass();
	HypercarClass = AHypercar::StaticClass();

	ActivePlayerCar = nullptr;
}

void ACarManager::BeginPlay()
{
	Super::BeginPlay();
}

TSubclassOf<ABaseCar> ACarManager::GetCarClassForType(ECarType CarType) const
{
	switch (CarType)
	{
	case ECarType::Supercar:
		return SupercarClass ? SupercarClass : TSubclassOf<ABaseCar>(ASupercar::StaticClass());

	case ECarType::TrackSupercar:
		return TrackSupercarClass ? TrackSupercarClass : TSubclassOf<ABaseCar>(ATrackSupercar::StaticClass());

	case ECarType::Hypercar:
		return HypercarClass ? HypercarClass : TSubclassOf<ABaseCar>(AHypercar::StaticClass());

	default:
		return SupercarClass ? SupercarClass : TSubclassOf<ABaseCar>(ASupercar::StaticClass());
	}
}

ABaseCar* ACarManager::SpawnCar(ECarType CarType, const FTransform& SpawnTransform)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("CarManager::SpawnCar failed: World is null."));
		return nullptr;
	}

	TSubclassOf<ABaseCar> ClassToSpawn = GetCarClassForType(CarType);
	if (!ClassToSpawn)
	{
		UE_LOG(LogTemp, Error, TEXT("CarManager::SpawnCar failed: Invalid car class for type %d."), static_cast<int32>(CarType));
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABaseCar* NewCar = World->SpawnActor<ABaseCar>(ClassToSpawn, SpawnTransform, SpawnParams);
	if (NewCar)
	{
		SpawnedFleet.Add(NewCar);
		OnCarSpawned.Broadcast(CarType, NewCar);
		UE_LOG(LogTemp, Display, TEXT("Spawned %s successfully at %s"), *NewCar->GetPerformanceStats().ModelName, *SpawnTransform.GetLocation().ToString());
	}

	return NewCar;
}

void ACarManager::PossessCar(APlayerController* PC, ABaseCar* TargetCar)
{
	if (!PC || !TargetCar)
	{
		UE_LOG(LogTemp, Warning, TEXT("CarManager::PossessCar invalid controller or target car."));
		return;
	}

	PC->Possess(TargetCar);
	ActivePlayerCar = TargetCar;
	UE_LOG(LogTemp, Display, TEXT("Possessed car %s by PlayerController."), *TargetCar->GetPerformanceStats().ModelName);
}

ABaseCar* ACarManager::SpawnAndPossessCar(ECarType CarType, const FTransform& SpawnTransform, APlayerController* PC)
{
	ABaseCar* NewCar = SpawnCar(CarType, SpawnTransform);
	if (NewCar && PC)
	{
		PossessCar(PC, NewCar);
	}
	return NewCar;
}
