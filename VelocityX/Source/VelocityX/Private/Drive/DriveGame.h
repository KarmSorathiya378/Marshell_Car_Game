#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/HUD.h"
#include "GameFramework/SaveGame.h"
#include "DriveGame.generated.h"

class UStaticMeshComponent;
class UInstancedStaticMeshComponent;
class UMaterialInterface;
class UCameraComponent;
class USpringArmComponent;
class UBoxComponent;
class UAudioComponent;
class USoundWaveProcedural;

UCLASS()
class VELOCITYX_API UDriveRecord : public USaveGame
{
 GENERATED_BODY()
public:
 UPROPERTY() float BestLap = 0.f;
};

UCLASS()
class VELOCITYX_API ADriveCircuit : public AActor
{
 GENERATED_BODY()
public:
 ADriveCircuit();
 virtual void BeginPlay() override;
 static FVector Point(float Angle);
 static FVector Tangent(float Angle);
 static float AngleAt(FVector Location);
 static constexpr int32 GateCount = 16;
 UPROPERTY() TArray<UInstancedStaticMeshComponent*> Batches;
 UInstancedStaticMeshComponent* Batch(const TCHAR* Name, const TCHAR* Shape, const TCHAR* Material, bool Collision = false);
 void Place(UInstancedStaticMeshComponent* Batch, FVector Location, FVector Scale, FRotator Rotation = FRotator::ZeroRotator);
};

UCLASS()
class VELOCITYX_API ADriveCar : public APawn
{
 GENERATED_BODY()
public:
 ADriveCar();
 virtual void BeginPlay() override;
 virtual void Tick(float Delta) override;
 virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
 virtual FVector GetVelocity() const override { return Motion; }
 UPROPERTY() UBoxComponent* Collision;
 UPROPERTY() USceneComponent* Visual;
 UPROPERTY() USpringArmComponent* Arm;
 UPROPERTY() UCameraComponent* Camera;
 UPROPERTY() TArray<UStaticMeshComponent*> Wheels;
 UPROPERTY() UAudioComponent* Audio;
 UPROPERTY() USoundWaveProcedural* EngineSound;
 UPROPERTY() UStaticMeshComponent* Body;
 float Speed = 0.f;
 float Steering = 0.f;
 float Boost = 1.f;
 float RaceTime = 0.f;
 float LapTime = 0.f;
 float BestLap = 0.f;
 float LastLap = 0.f;
 float Countdown = 3.f;
 float NoticeTime = 0.f;
 int32 Lap = 1;
 int32 NextGate = 1;
 int32 CarType = 0;
 int32 Quality = 2;
 bool bStarted = false;
 bool bFinished = false;
 bool bOffroad = false;
 bool bBoosting = false;
 FString Notice;
 FString RecordSlot = TEXT("VelocityX_Record");
 FVector Motion = FVector::ZeroVector;
 void ResetCar();
 void Restart();
 void Start();
 void Pause();
 void ChangeCamera();
 void ChangeQuality();
 void SelectGT();
 void SelectTrack();
 void SelectHyper();
 void Select(int32 Type);
 void UpdateRace(FVector Previous, FVector Current);
 void Step(float Dt, float Throttle, float Brake, float Steer, bool Handbrake, bool Nitro);
 const TCHAR* CarName() const;
private:
 float Accumulator = 0.f;
 float WheelSpin = 0.f;
 float AudioPhase = 0.f;
 float SmokeTime = 0.f;
 int32 CameraMode = 0;
 bool bSmokeTest = false;
 bool bSmokePassed = false;
 UStaticMeshComponent* Part(const TCHAR* Name, const TCHAR* Shape, const TCHAR* Material, FVector Location, FVector Scale, FRotator Rotation = FRotator::ZeroRotator);
 void UpdateAudio();
};

UCLASS()
class VELOCITYX_API ADriveHUD : public AHUD
{
 GENERATED_BODY()
public:
 virtual void DrawHUD() override;
private:
 float Unit = 1.f;
 void Label(const FString& Text, float X, float Y, float Size, FLinearColor Color);
 void Panel(float X, float Y, float W, float H, FLinearColor Color);
};

UCLASS()
class VELOCITYX_API ADriveGameMode : public AGameModeBase
{
 GENERATED_BODY()
public:
 ADriveGameMode();
 virtual void InitGame(const FString& MapName, const FString& Options, FString& Error) override;
 virtual void RestartPlayer(AController* Player) override;
};
