#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "DriveGame.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDriveRaceTest,"VelocityX.Drive.CheckpointOrder",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDriveRaceTest::RunTest(const FString& Parameters)
{
 UWorld* World=UWorld::CreateWorld(EWorldType::Game,false);
 auto* Car=World->SpawnActor<ADriveCar>(); Car->Speed=2000;
 auto Cross=[Car](int32 Gate,bool Forward)
 {
  float A=Gate*2.f*PI/16.f; FVector P=ADriveCircuit::Point(A),T=ADriveCircuit::Tangent(A)*100;
  Car->UpdateRace(P+(Forward?-T:T),P+(Forward?T:-T));
 };
 Cross(4,true); TestEqual(TEXT("Cannot skip checkpoints"),Car->NextGate,1);
 Cross(1,false); TestEqual(TEXT("Reverse crossing rejected"),Car->NextGate,1);
 Cross(1,true); TestEqual(TEXT("Forward crossing advances once"),Car->NextGate,2);
 Cross(1,true); TestEqual(TEXT("Duplicate crossing rejected"),Car->NextGate,2);
 Car->ResetCar(); TestEqual(TEXT("Recovery preserves next gate"),Car->NextGate,2);
 TestTrue(TEXT("Recovery stops car"),FMath::IsNearlyZero(Car->Speed));
 TestTrue(TEXT("Recovery returns to last valid gate"),FVector::Dist2D(Car->GetActorLocation(),ADriveCircuit::Point(2*PI/16))<1);
 Car->Restart(); TestEqual(TEXT("Restart resets lap"),Car->Lap,1); TestEqual(TEXT("Restart resets checkpoints"),Car->NextGate,1);
 Car->RecordSlot=TEXT("VelocityX_Automation_Record"); Car->Speed=2000;
 for(int32 L=0;L<3;++L) { Car->LapTime=45.f+L; for(int32 Gate=1;Gate<=16;++Gate) Cross(Gate%16,true); }
 TestTrue(TEXT("Three valid laps finish the race"),Car->bFinished);
 TestEqual(TEXT("Best lap is persisted correctly"),Car->BestLap,45.f);
 TestTrue(TEXT("Finished car is stopped"),FMath::IsNearlyZero(Car->Speed));
 UGameplayStatics::DeleteGameInSlot(Car->RecordSlot,0);
 Car->Destroy(); World->DestroyWorld(false); return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDriveDynamicsTest,"VelocityX.Drive.AccelerationBrakingSteering",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDriveDynamicsTest::RunTest(const FString& Parameters)
{
 UWorld* World=UWorld::CreateWorld(EWorldType::Game,false); auto* Car=World->SpawnActor<ADriveCar>(); Car->ResetCar();
 const FVector Start=Car->GetActorLocation();
 for(int32 I=0;I<240;++I) Car->Step(1.f/120,1,0,0,false,false);
 TestTrue(TEXT("Throttle produces forward speed"),Car->Speed>3000);
 TestTrue(TEXT("Throttle moves the chassis"),FVector::Dist2D(Start,Car->GetActorLocation())>2500);
 const float BeforeBrake=Car->Speed;
 for(int32 I=0;I<60;++I) Car->Step(1.f/120,0,1,0,false,false);
 TestTrue(TEXT("Braking reduces speed"),Car->Speed<BeforeBrake*.5f);
 Car->ResetCar(); for(int32 I=0;I<60;++I) Car->Step(1.f/120,0,1,0,false,false);
 TestTrue(TEXT("Brake from rest engages reverse"),Car->Speed<0);
 Car->ResetCar(); Car->Speed=2000; const float BeforeYaw=Car->GetActorRotation().Yaw;
 for(int32 I=0;I<30;++I) Car->Step(1.f/120,1,0,1,false,false);
 TestTrue(TEXT("Steering changes heading"),FMath::Abs(Car->GetActorRotation().Yaw-BeforeYaw)>1);
 Car->ResetCar(); Car->Speed=2000;
 for(int32 I=0;I<30;++I) Car->Step(1.f/120,1,0,0,false,true);
 TestTrue(TEXT("Boost consumes charge"),Car->Boost<1);
 Car->Destroy(); World->DestroyWorld(false); return true;
}
#endif
