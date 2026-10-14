// Copyright VELOCITY X Racing. All Rights Reserved.

#include "CarManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ACarManager::ACarManager()
{
	PrimaryActorTick.bCanEverTick = true;

	SupercarClass = ASupercar::StaticClass();
	TrackSupercarClass = ATrackSupercar::StaticClass();
	HypercarClass = AHypercar::StaticClass();

	GlobalSurfaceType = ESurfaceType::DryAsphalt;
	GlobalAirDensity = 1.225f;
	bBenchmarkRunning = false;
	BenchmarkTimer = 0.0f;
	bPassed100Kmh = false;
	bPassed200Kmh = false;
}

void ACarManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("[VELOCITY X] CarManager started and initialized."));
}

void ACarManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bBenchmarkRunning)
	{
		UpdateBenchmark(DeltaTime);
	}
}

ABaseCar* ACarManager::SpawnVehicle(TSubclassOf<ABaseCar> VehicleClass, const FTransform& SpawnTransform)
{
	if (!GetWorld() || !VehicleClass)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABaseCar* SpawnedCar = GetWorld()->SpawnActor<ABaseCar>(VehicleClass, SpawnTransform, SpawnParams);
	if (SpawnedCar)
	{
		UE_LOG(LogTemp, Display, TEXT("[VELOCITY X] Spawned vehicle: %s"), *SpawnedCar->GetPhysicsSettings().VehicleName);
	}

	return SpawnedCar;
}

ABaseCar* ACarManager::SwitchActiveVehicle(const FString& VehicleType, APlayerController* PC)
{
	TSubclassOf<ABaseCar> ChosenClass = nullptr;

	if (VehicleType.Equals(TEXT("Supercar"), ESearchCase::IgnoreCase))
	{
		ChosenClass = SupercarClass;
	}
	else if (VehicleType.Equals(TEXT("TrackSupercar"), ESearchCase::IgnoreCase) || VehicleType.Equals(TEXT("TrackSpec"), ESearchCase::IgnoreCase))
	{
		ChosenClass = TrackSupercarClass;
	}
	else if (VehicleType.Equals(TEXT("Hypercar"), ESearchCase::IgnoreCase))
	{
		ChosenClass = HypercarClass;
	}
	else
	{
		ChosenClass = SupercarClass;
	}

	FTransform SpawnTransform = FTransform::Identity;
	if (ActiveVehicle)
	{
		SpawnTransform = ActiveVehicle->GetActorTransform();
		// Unpossess and destroy old vehicle
		if (PC && PC->GetPawn() == ActiveVehicle)
		{
			PC->UnPossess();
		}
		ActiveVehicle->Destroy();
		ActiveVehicle = nullptr;
	}

	ActiveVehicle = SpawnVehicle(ChosenClass, SpawnTransform);

	if (ActiveVehicle && PC)
	{
		PC->Possess(ActiveVehicle);
		PC->SetViewTarget(ActiveVehicle);
	}

	return ActiveVehicle;
}

ABaseCar* ACarManager::SpawnVehicleByType(ECarType Type, const FTransform& Transform)
{
	switch (Type)
	{
	case ECarType::Supercar:
		return SpawnVehicle(SupercarClass, Transform);
	case ECarType::TrackSpec:
		return SpawnVehicle(TrackSupercarClass, Transform);
	case ECarType::Hypercar:
		return SpawnVehicle(HypercarClass, Transform);
	default:
		return SpawnVehicle(SupercarClass, Transform);
	}
}

ABaseCar* ACarManager::SwitchActiveVehicleByType(ECarType Type, APlayerController* PC)
{
	switch (Type)
	{
	case ECarType::Supercar:
		return SwitchActiveVehicle(TEXT("Supercar"), PC);
	case ECarType::TrackSpec:
		return SwitchActiveVehicle(TEXT("TrackSupercar"), PC);
	case ECarType::Hypercar:
		return SwitchActiveVehicle(TEXT("Hypercar"), PC);
	default:
		return SwitchActiveVehicle(TEXT("Supercar"), PC);
	}
}

ABaseCar* ACarManager::SwitchActiveVehicleByIndex(int32 VehicleIndex, APlayerController* PC)
{
	switch (VehicleIndex)
	{
	case 0:
		return SwitchActiveVehicle(TEXT("Supercar"), PC);
	case 1:
		return SwitchActiveVehicle(TEXT("TrackSupercar"), PC);
	case 2:
		return SwitchActiveVehicle(TEXT("Hypercar"), PC);
	default:
		return SwitchActiveVehicle(TEXT("Supercar"), PC);
	}
}

FVehicleDisplayStats ACarManager::GetCarStats(ECarType Type) const
{
	FVehicleDisplayStats Stats;

	switch (Type)
	{
	case ECarType::Supercar:
		Stats.VehicleName = TEXT("Apex GT3-R (Supercar)");
		Stats.TopSpeedKmh = 320.0f;
		Stats.Acceleration0To100Seconds = 3.5f;
		Stats.Horsepower = 500.0f;
		Stats.WeightKg = 1500.0f;
		Stats.HandlingRating = 88.0f;
		Stats.BrakingRating = 86.0f;
		break;

	case ECarType::TrackSpec:
		Stats.VehicleName = TEXT("Veloce Senna-R (Track Supercar)");
		Stats.TopSpeedKmh = 355.0f;
		Stats.Acceleration0To100Seconds = 2.8f;
		Stats.Horsepower = 720.0f;
		Stats.WeightKg = 1200.0f;
		Stats.HandlingRating = 98.0f; // Extreme downforce & track grip
		Stats.BrakingRating = 95.0f;  // Race airbrake
		break;

	case ECarType::Hypercar:
		Stats.VehicleName = TEXT("Titan Chiron-X (Hypercar)");
		Stats.TopSpeedKmh = 420.0f;
		Stats.Acceleration0To100Seconds = 2.4f;
		Stats.Horsepower = 1500.0f;
		Stats.WeightKg = 1900.0f;
		Stats.HandlingRating = 85.0f;
		Stats.BrakingRating = 96.0f;  // Massive carbon-silicon brakes + airbrake
		break;

	default:
		break;
	}

	return Stats;
}

void ACarManager::SetGlobalSurfaceType(ESurfaceType NewSurface)
{
	GlobalSurfaceType = NewSurface;

	if (ActiveVehicle)
	{
		float Friction = ABaseCar::GetSurfaceFrictionCoefficient(NewSurface);
		FVehiclePhysicsConfig Cfg = ActiveVehicle->GetPhysicsSettings();
		Cfg.Tires.FrictionMultiplier = Friction;
		ActiveVehicle->SetPhysicsSettings(Cfg);
	}

	UE_LOG(LogTemp, Display, TEXT("[VELOCITY X] Global track surface set to %d (Friction: %.2f)"),
		static_cast<int32>(NewSurface), ABaseCar::GetSurfaceFrictionCoefficient(NewSurface));
}

void ACarManager::SetAirDensity(float DensityKgM3)
{
	GlobalAirDensity = FMath::Clamp(DensityKgM3, 0.5f, 2.0f);

	if (ActiveVehicle)
	{
		FVehiclePhysicsConfig Cfg = ActiveVehicle->GetPhysicsSettings();
		Cfg.Aerodynamics.AirDensity = GlobalAirDensity;
		ActiveVehicle->SetPhysicsSettings(Cfg);
	}

	UE_LOG(LogTemp, Display, TEXT("[VELOCITY X] Ambient air density set to %.3f kg/m^3"), GlobalAirDensity);
}

void ACarManager::StartBenchmarkTest()
{
	if (!ActiveVehicle)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CarManager] Cannot start benchmark: No active vehicle!"));
		return;
	}

	ActiveVehicle->ResetVehicle();
	BenchmarkResult = FVehicleBenchmarkResult();
	BenchmarkResult.VehicleName = ActiveVehicle->GetPhysicsSettings().VehicleName;
	BenchmarkResult.bTestCompleted = false;

	bBenchmarkRunning = true;
	BenchmarkTimer = 0.0f;
	bPassed100Kmh = false;
	bPassed200Kmh = false;
	BenchmarkStartLocation = ActiveVehicle->GetActorLocation();

	// Full throttle for benchmark launch
	ActiveVehicle->ApplyThrottle(1.0f);
	ActiveVehicle->ApplyBrake(0.0f);

	UE_LOG(LogTemp, Display, TEXT("[CarManager] Benchmark STARTED for %s"), *BenchmarkResult.VehicleName);
}

void ACarManager::ResetBenchmarkTest()
{
	bBenchmarkRunning = false;
	BenchmarkTimer = 0.0f;
	BenchmarkResult = FVehicleBenchmarkResult();
}

void ACarManager::UpdateBenchmark(float DeltaTime)
{
	if (!ActiveVehicle)
	{
		bBenchmarkRunning = false;
		return;
	}

	BenchmarkTimer += DeltaTime;
	FVehicleTelemetryData Telemetry = ActiveVehicle->GetTelemetry();

	if (Telemetry.SpeedKmh > BenchmarkResult.MaxSpeedAchievedKmh)
	{
		BenchmarkResult.MaxSpeedAchievedKmh = Telemetry.SpeedKmh;
	}

	if (FMath::Abs(Telemetry.LateralGForce) > BenchmarkResult.PeakLateralG)
	{
		BenchmarkResult.PeakLateralG = FMath::Abs(Telemetry.LateralGForce);
	}

	// 0-100 km/h split
	if (!bPassed100Kmh && Telemetry.SpeedKmh >= 100.0f)
	{
		bPassed100Kmh = true;
		BenchmarkResult.Time0To100Kmh = BenchmarkTimer;
		UE_LOG(LogTemp, Display, TEXT("[CarManager] 0-100 km/h achieved in %.3f seconds!"), BenchmarkResult.Time0To100Kmh);
	}

	// 0-200 km/h split
	if (!bPassed200Kmh && Telemetry.SpeedKmh >= 200.0f)
	{
		bPassed200Kmh = true;
		BenchmarkResult.Time0To200Kmh = BenchmarkTimer;
		UE_LOG(LogTemp, Display, TEXT("[CarManager] 0-200 km/h achieved in %.3f seconds!"), BenchmarkResult.Time0To200Kmh);
	}

	// End test after 15 seconds or when top speed plateau is reached
	if (BenchmarkTimer >= 15.0f || (bPassed200Kmh && BenchmarkTimer >= 12.0f))
	{
		bBenchmarkRunning = false;
		BenchmarkResult.bTestCompleted = true;
		ActiveVehicle->ApplyThrottle(0.0f);
		ActiveVehicle->ApplyBrake(1.0f); // Measure emergency braking from top speed

		UE_LOG(LogTemp, Display, TEXT("[CarManager] Benchmark COMPLETE for %s. 0-100: %.2fs, Top Speed: %.1f km/h"),
			*BenchmarkResult.VehicleName, BenchmarkResult.Time0To100Kmh, BenchmarkResult.MaxSpeedAchievedKmh);
	}
}

ACarManager* ACarManager::GetCarManager(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	if (!World)
	{
		return nullptr;
	}

	TArray<AActor*> FoundManagers;
	UGameplayStatics::GetAllActorsOfClass(World, ACarManager::StaticClass(), FoundManagers);

	if (FoundManagers.Num() > 0)
	{
		return Cast<ACarManager>(FoundManagers[0]);
	}

	return nullptr;
}
