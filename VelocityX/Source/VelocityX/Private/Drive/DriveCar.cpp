#include "DriveGame.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "UnrealClient.h"
#include "Materials/MaterialInterface.h"
#include "Sound/SoundWaveProcedural.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "HAL/PlatformMisc.h"

UStaticMeshComponent* ADriveCar::Part(const TCHAR* Name, const TCHAR* Shape, const TCHAR* Material, FVector P, FVector S, FRotator R)
{
 auto* C=CreateDefaultSubobject<UStaticMeshComponent>(Name);
 C->SetupAttachment(Visual);
 C->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,*FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"),Shape,Shape)));
 C->SetMaterial(0,LoadObject<UMaterialInterface>(nullptr,*FString::Printf(TEXT("/Game/Drive/Materials/M_%s.M_%s"),Material,Material)));
 C->SetRelativeLocation(P); C->SetRelativeScale3D(S); C->SetRelativeRotation(R);
 C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 return C;
}
ADriveCar::ADriveCar()
{
 PrimaryActorTick.bCanEverTick=true;
 Collision=CreateDefaultSubobject<UBoxComponent>(TEXT("ChassisCollision"));
 RootComponent=Collision; Collision->SetBoxExtent(FVector(205,91,39));
 Collision->SetCollisionProfileName(TEXT("Pawn"));
 Visual=CreateDefaultSubobject<USceneComponent>(TEXT("BodyRoot")); Visual->SetupAttachment(RootComponent);
 Body=Part(TEXT("Body"),TEXT("Cube"),TEXT("Paint"),FVector(0,0,0),FVector(4.5f,1.85f,.5f));
 if(auto* Mesh=LoadObject<UStaticMesh>(nullptr,TEXT("/Game/Drive/Meshes/SM_CoupeBody.SM_CoupeBody"))) { Body->SetStaticMesh(Mesh); Body->SetRelativeScale3D(FVector(1)); }
 auto* Cabin=Part(TEXT("Cabin"),TEXT("Cube"),TEXT("Glass"),FVector(-22,0,48),FVector(1.65f,1.48f,.58f));
 if(auto* Mesh=LoadObject<UStaticMesh>(nullptr,TEXT("/Game/Drive/Meshes/SM_CoupeGlass.SM_CoupeGlass"))) { Cabin->SetStaticMesh(Mesh); Cabin->SetRelativeLocation(FVector::ZeroVector); Cabin->SetRelativeScale3D(FVector(1)); }
 Part(TEXT("Roof"),TEXT("Cube"),TEXT("Paint"),FVector(-28,0,87),FVector(.8f,1.14f,.04f));
 Part(TEXT("Splitter"),TEXT("Cube"),TEXT("Dark"),FVector(212,0,-23),FVector(.4f,2.03f,.09f));
 Part(TEXT("Diffuser"),TEXT("Cube"),TEXT("Dark"),FVector(-212,0,-23),FVector(.45f,1.95f,.15f));
 Part(TEXT("Wing"),TEXT("Cube"),TEXT("Dark"),FVector(-196,0,80),FVector(.55f,2.18f,.09f),FRotator(5,0,0));
 for(int32 I=0;I<2;++I)
 {
  float Side=I ? 1.f:-1.f;
  Part(*FString::Printf(TEXT("WingSupport%d"),I),TEXT("Cube"),TEXT("Dark"),FVector(-196,Side*62,50),FVector(.12f,.08f,.6f));
  Part(*FString::Printf(TEXT("Sill%d"),I),TEXT("Cube"),TEXT("Dark"),FVector(0,Side*95,-22),FVector(3.5f,.1f,.12f));
  Part(*FString::Printf(TEXT("Headlamp%d"),I),TEXT("Cube"),TEXT("WhiteGlow"),FVector(234,Side*62,12),FVector(.035f,.5f,.1f));
  Part(*FString::Printf(TEXT("TailLight%d"),I),TEXT("Cube"),TEXT("RedGlow"),FVector(-227,Side*57,13),FVector(.035f,.6f,.09f));
  Part(*FString::Printf(TEXT("Stripe%d"),I),TEXT("Cube"),TEXT("Dark"),FVector(151,Side*24,39),FVector(1.23f,.15f,.025f),FRotator(-8,0,0));
 }
 for(int32 I=0;I<4;++I)
 {
  FVector P(I<2 ? 137.f:-141.f,I%2 ? 96.f:-96.f,-20);
  auto* Wheel=Part(*FString::Printf(TEXT("Wheel%d"),I),TEXT("Cylinder"),TEXT("Rubber"),P,FVector(.7f,.7f,.28f),FRotator(0,0,90)); Wheels.Add(Wheel);
  auto* Rim=Part(*FString::Printf(TEXT("Rim%d"),I),TEXT("Cylinder"),TEXT("Metal"),P+FVector(0,I%2 ? 15.f:-15.f,0),FVector(.47f,.47f,.02f),FRotator(0,0,90));
  Rim->SetupAttachment(Wheel);
  Rim->SetRelativeLocation(FVector(0,0,I%2?-54.f:54.f)); Rim->SetRelativeRotation(FRotator::ZeroRotator); Rim->SetRelativeScale3D(FVector(.67f,.67f,.072f));
 }
 Arm=CreateDefaultSubobject<USpringArmComponent>(TEXT("ChaseArm")); Arm->SetupAttachment(RootComponent);
 Arm->TargetArmLength=720; Arm->SetRelativeLocation(FVector(0,0,130)); Arm->SetRelativeRotation(FRotator(-12,0,0));
 Arm->bEnableCameraLag=true; Arm->CameraLagSpeed=12; Arm->bEnableCameraRotationLag=true; Arm->CameraRotationLagSpeed=10;
 Arm->bInheritRoll=false; Arm->bInheritPitch=false;
 Camera=CreateDefaultSubobject<UCameraComponent>(TEXT("DriveCamera")); Camera->SetupAttachment(Arm,USpringArmComponent::SocketName); Camera->FieldOfView=82;
 Audio=CreateDefaultSubobject<UAudioComponent>(TEXT("EngineAudio")); Audio->SetupAttachment(RootComponent); Audio->bAutoActivate=false;
}
void ADriveCar::BeginPlay()
{
 Super::BeginPlay();
 if(auto* PC=Cast<APlayerController>(Controller)) { PC->SetInputMode(FInputModeGameOnly()); PC->bShowMouseCursor=false; }
 if(UGameplayStatics::DoesSaveGameExist(RecordSlot,0))
  if(auto* Record=Cast<UDriveRecord>(UGameplayStatics::LoadGameFromSlot(RecordSlot,0))) BestLap=Record->BestLap;
 EngineSound=NewObject<USoundWaveProcedural>(this); EngineSound->SetSampleRate(22050); EngineSound->NumChannels=1;
 EngineSound->Duration=INDEFINITELY_LOOPING_DURATION; EngineSound->SoundGroup=SOUNDGROUP_Effects; Audio->SetSound(EngineSound); Audio->SetVolumeMultiplier(.22f); Audio->Play();
 bSmokeTest=FParse::Param(FCommandLine::Get(),TEXT("DriveSmokeTest"));
 if(bSmokeTest) { Start(); Countdown=0; }
 UE_LOG(LogTemp,Display,TEXT("VELOCITY_READY: car spawned and input ready"));
}
void ADriveCar::SetupPlayerInputComponent(UInputComponent* I)
{
 Super::SetupPlayerInputComponent(I);
 I->BindKey(EKeys::Enter,IE_Pressed,this,&ADriveCar::Start);
 I->BindKey(EKeys::Gamepad_FaceButton_Bottom,IE_Pressed,this,&ADriveCar::Start);
 I->BindKey(EKeys::R,IE_Pressed,this,&ADriveCar::ResetCar);
 I->BindKey(EKeys::Gamepad_Special_Left,IE_Pressed,this,&ADriveCar::ResetCar);
 I->BindKey(EKeys::F5,IE_Pressed,this,&ADriveCar::Restart).bExecuteWhenPaused=true;
 I->BindKey(EKeys::C,IE_Pressed,this,&ADriveCar::ChangeCamera);
 I->BindKey(EKeys::Gamepad_RightThumbstick,IE_Pressed,this,&ADriveCar::ChangeCamera);
 I->BindKey(EKeys::Escape,IE_Pressed,this,&ADriveCar::Pause).bExecuteWhenPaused=true;
 I->BindKey(EKeys::P,IE_Pressed,this,&ADriveCar::Pause).bExecuteWhenPaused=true;
 I->BindKey(EKeys::Gamepad_Special_Right,IE_Pressed,this,&ADriveCar::Pause).bExecuteWhenPaused=true;
 I->BindKey(EKeys::F1,IE_Pressed,this,&ADriveCar::ChangeQuality).bExecuteWhenPaused=true;
 I->BindKey(EKeys::One,IE_Pressed,this,&ADriveCar::SelectGT);
 I->BindKey(EKeys::Two,IE_Pressed,this,&ADriveCar::SelectTrack);
 I->BindKey(EKeys::Three,IE_Pressed,this,&ADriveCar::SelectHyper);
}
const TCHAR* ADriveCar::CarName() const { return CarType==0 ? TEXT("APEX GT") : CarType==1 ? TEXT("VELOCE R") : TEXT("TITAN X"); }
void ADriveCar::SelectGT(){ Select(0); } void ADriveCar::SelectTrack(){ Select(1); } void ADriveCar::SelectHyper(){ Select(2); }
void ADriveCar::Select(int32 Type)
{
 if(bStarted) return;
 CarType=Type;
 const TCHAR* Mat=Type==0?TEXT("Paint"):Type==1?TEXT("Red"):TEXT("White");
 Body->SetMaterial(0,LoadObject<UMaterialInterface>(nullptr,*FString::Printf(TEXT("/Game/Drive/Materials/M_%s.M_%s"),Mat,Mat)));
}
void ADriveCar::Start() { if(bFinished) Restart(); if(!bStarted) { bStarted=true; Countdown=3.f; } }
void ADriveCar::Pause() { if(auto* PC=Cast<APlayerController>(Controller)) { PC->SetPause(!PC->IsPaused()); } }
void ADriveCar::Restart()
{
 if(auto* PC=Cast<APlayerController>(Controller)) PC->SetPause(false);
 bStarted=false; bFinished=false; Lap=1; NextGate=1; LapTime=RaceTime=LastLap=0; Countdown=3; Boost=1; NoticeTime=0;
 ResetCar();
}
void ADriveCar::ResetCar()
{
 const float A=(NextGate-1)*2.f*PI/ADriveCircuit::GateCount;
 SetActorLocationAndRotation(ADriveCircuit::Point(A)+FVector(0,0,57),ADriveCircuit::Tangent(A).Rotation(),false,nullptr,ETeleportType::TeleportPhysics);
 Speed=0; Motion=FVector::ZeroVector; Steering=0; Accumulator=0;
 // Reset to the last valid checkpoint; never advance race progress on recovery.
}
void ADriveCar::ChangeCamera()
{
 CameraMode=(CameraMode+1)%3;
 Arm->TargetArmLength=CameraMode==0 ? 720:CameraMode==1 ? 1050:0;
 Arm->SetRelativeLocation(CameraMode==2 ? FVector(195,0,95):FVector(0,0,CameraMode==1 ? 280:130));
 Arm->SetRelativeRotation(FRotator(CameraMode==1 ? -18:-12,0,0));
 Arm->bEnableCameraLag=CameraMode!=2;
}
void ADriveCar::ChangeQuality()
{
 Quality=(Quality+1)%3;
 auto* S=GEngine->GetGameUserSettings(); S->SetOverallScalabilityLevel(Quality+1);
 S->SetResolutionScaleValueEx(Quality==0?75:100); S->SetFrameRateLimit(120); S->ApplySettings(false);
 Notice=Quality==0 ? TEXT("PERFORMANCE / 75% resolution") : Quality==1 ? TEXT("BALANCED / native resolution") : TEXT("HIGH / native resolution"); NoticeTime=3;
}
void ADriveCar::Step(float Dt,float Throttle,float Brake,float Steer,bool Handbrake,bool Nitro)
{
 const FVector Old=GetActorLocation(); const float A=ADriveCircuit::AngleAt(Old);
 bOffroad=FVector::Dist2D(Old,ADriveCircuit::Point(A))>1190;
 const float MaxSpeed=CarType==0?8800:CarType==1?9100:10500;
 bBoosting=Nitro && Boost>.01f && Throttle>.1f && Speed>500 && !bOffroad;
 Boost=FMath::Clamp(Boost+(bBoosting ? -.22f:.09f)*Dt,0.f,1.f);
 float Accel=CarType==0?2600:CarType==1?3000:3400;
 if(Throttle>.01f) Speed+=Throttle*(Speed<0 ? 4800:Accel*FMath::Clamp(1.f-Speed/(MaxSpeed*(bBoosting?1.24f:1.f)),.08f,1.f))*Dt;
 if(Brake>.01f) Speed-=Brake*(Speed>0 ? 6200:1500)*Dt;
 if(Throttle<.01f && Brake<.01f) Speed=FMath::FInterpConstantTo(Speed,0.f,Dt,130.f+FMath::Abs(Speed)*.018f);
 if(Handbrake) Speed=FMath::FInterpConstantTo(Speed,0.f,Dt,3200);
 if(bOffroad) Speed=FMath::FInterpConstantTo(Speed,0.f,Dt,1700);
 if(bBoosting) Speed+=1700*Dt;
 Speed=FMath::Clamp(Speed,-1800.f,MaxSpeed*(bBoosting?1.24f:1.f));
 Steering=FMath::FInterpTo(Steering,Steer,Dt,8.f);
 float Angle=Steering*(30.f/(1.f+FMath::Square(Speed/1900.f)));
 float Yaw=Speed/280.f*FMath::Tan(FMath::DegreesToRadians(Angle));
 if(Handbrake) Yaw*=1.5f;
 FRotator R=GetActorRotation(); R.Yaw+=FMath::RadiansToDegrees(Yaw)*Dt;
 SetActorRotation(R);
 FVector Target=GetActorForwardVector()*Speed;
 Motion=FMath::VInterpTo(Motion,Target,Dt,Handbrake ? 2.f : CarType==1 ? 13.f:9.f);
 FHitResult Hit; AddActorWorldOffset(Motion*Dt,true,&Hit);
 if(Hit.bBlockingHit)
 {
  FVector Slide=FVector::VectorPlaneProject(Motion,Hit.Normal);
  Speed*=.55f; Motion=Slide*.6f; AddActorWorldOffset(Slide*Dt*(1.f-Hit.Time),true);
 }
 // This flat circuit uses a fixed-height arcade chassis; no missing skeletal/physics asset dependency.
 FVector P=GetActorLocation(); P.Z=57; SetActorLocation(P,false);
 UpdateRace(Old,P);
}
void ADriveCar::UpdateRace(FVector Previous,FVector Current)
{
 const float A=(NextGate%ADriveCircuit::GateCount)*2.f*PI/ADriveCircuit::GateCount;
 const FVector Center=ADriveCircuit::Point(A), Forward=ADriveCircuit::Tangent(A);
 const float Before=FVector::DotProduct(Previous-Center,Forward), After=FVector::DotProduct(Current-Center,Forward);
 if(Before<0 && After>=0 && FVector::Dist2D(Current,Center)<1400 && Speed>0)
 {
  ++NextGate;
  if(NextGate>ADriveCircuit::GateCount)
  {
   LastLap=LapTime;
   if(BestLap<=0 || LastLap<BestLap)
   {
    BestLap=LastLap;
    auto* Record=Cast<UDriveRecord>(UGameplayStatics::CreateSaveGameObject(UDriveRecord::StaticClass())); Record->BestLap=BestLap;
    UGameplayStatics::SaveGameToSlot(Record,RecordSlot,0);
    Notice=TEXT("NEW PERSONAL BEST");
   }
   else Notice=TEXT("LAP COMPLETE");
   NoticeTime=4; LapTime=0; NextGate=1;
   if(Lap==3) { bFinished=true; Speed=0; Motion=FVector::ZeroVector; } else ++Lap;
  }
 }
}
void ADriveCar::Tick(float Delta)
{
 Super::Tick(Delta);
 auto* PC=Cast<APlayerController>(Controller); if(!PC) return;
 NoticeTime=FMath::Max(0.f,NoticeTime-Delta);
 if(bStarted && !bFinished)
 {
  if(Countdown>0) Countdown=FMath::Max(0.f,Countdown-Delta);
  else
  {
   float T=(PC->IsInputKeyDown(EKeys::W)||PC->IsInputKeyDown(EKeys::Up))?1.f:0.f;
   float B=(PC->IsInputKeyDown(EKeys::S)||PC->IsInputKeyDown(EKeys::Down))?1.f:0.f;
   float S=(PC->IsInputKeyDown(EKeys::D)||PC->IsInputKeyDown(EKeys::Right)?1.f:0.f)-(PC->IsInputKeyDown(EKeys::A)||PC->IsInputKeyDown(EKeys::Left)?1.f:0.f);
   T=FMath::Max(T,PC->GetInputAnalogKeyState(EKeys::Gamepad_RightTriggerAxis)); B=FMath::Max(B,PC->GetInputAnalogKeyState(EKeys::Gamepad_LeftTriggerAxis));
   float Stick=PC->GetInputAnalogKeyState(EKeys::Gamepad_LeftX); if(FMath::Abs(Stick)>.15f) S=Stick;
   if(bSmokeTest) T=1;
   const bool H=PC->IsInputKeyDown(EKeys::SpaceBar)||PC->IsInputKeyDown(EKeys::Gamepad_FaceButton_Right);
   const bool N=PC->IsInputKeyDown(EKeys::LeftShift)||PC->IsInputKeyDown(EKeys::Gamepad_FaceButton_Left);
   // Bound catch-up work after a hitch. Physics runs at 120 Hz across rendering frame rates.
   RaceTime+=Delta; LapTime+=Delta;
   Accumulator=FMath::Min(Accumulator+Delta,.1f);
   while(Accumulator>=1.f/120.f && !bFinished)
   {
    const float Dt=1.f/120.f; Step(Dt,T,B,S,H,N); Accumulator-=Dt;
   }
  }
 }
 WheelSpin=FMath::Fmod(WheelSpin+Speed*Delta/35.f*180.f/PI,360.f);
 for(int32 I=0;I<Wheels.Num();++I) Wheels[I]->SetRelativeRotation((FRotator(0,I<2?Steering*24:0,90).Quaternion()*FQuat(FVector::UpVector,FMath::DegreesToRadians(WheelSpin))));
 Visual->SetRelativeRotation(FRotator(0,0,-Steering*FMath::Clamp(Speed/2200.f,-2.f,2.f)));
 Camera->SetFieldOfView(FMath::FInterpTo(Camera->FieldOfView,82.f+FMath::Abs(Speed)/1000.f+(bBoosting?8:0),Delta,4));
 UpdateAudio();
 if(FParse::Param(FCommandLine::Get(),TEXT("DriveCapture")))
 {
  SmokeTime+=Delta;
  if(SmokeTime>3 && !bSmokePassed) { bSmokePassed=true; FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("Screenshots/DriveMenu.png"),true,false); }
  if(SmokeTime>5) FPlatformMisc::RequestExit(false);
 }
 if(bSmokeTest)
 {
  SmokeTime+=Delta;
  static int32 MeasuredFrames=0; static double MeasuredSeconds=0;
  if(SmokeTime>1) { ++MeasuredFrames; MeasuredSeconds+=Delta; }
  if(SmokeTime>2.f && !bSmokePassed)
  {
   bSmokePassed=Speed>1000 && FVector::Dist2D(GetActorLocation(),ADriveCircuit::Point(0))>700;
   if(bSmokePassed) { FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("Screenshots/DriveSmoke.png"),true,false); UE_LOG(LogTemp,Display,TEXT("DRIVE_SMOKE_PASS: speed=%.1f distance=%.1f"),Speed,FVector::Dist2D(GetActorLocation(),ADriveCircuit::Point(0))); FFileHelper::SaveStringToFile(TEXT("PASS: possessed car accelerates and moves on generated circuit"),*(FPaths::ProjectSavedDir()/TEXT("DriveSmokeResult.txt"))); }
  }
  if(SmokeTime>5.f) { UE_LOG(LogTemp,Display,TEXT("DRIVE_PERF_SAMPLE: %.1f FPS over %.2f seconds"),MeasuredFrames/FMath::Max(MeasuredSeconds,.001),MeasuredSeconds); if(!bSmokePassed) UE_LOG(LogTemp,Error,TEXT("DRIVE_SMOKE_FAIL")); FPlatformMisc::RequestExit(false); }
 }
}
void ADriveCar::UpdateAudio()
{
 if(!EngineSound || EngineSound->GetAvailableAudioByteCount()>4096) return;
 TArray<int16> Samples; Samples.SetNumUninitialized(4096);
 const float GearSpeed=FMath::Fmod(FMath::Abs(Speed),1800.f)/1800.f;
 const float Frequency=45.f+GearSpeed*65.f+FMath::Abs(Speed)*.005f;
 for(int32 I=0;I<Samples.Num();++I)
 {
  AudioPhase=FMath::Fmod(AudioPhase+2.f*PI*Frequency/22050.f,2.f*PI);
  Samples[I]=static_cast<int16>((FMath::Sin(AudioPhase)+.3f*FMath::Sin(AudioPhase*2.f)+.13f*FMath::Sin(AudioPhase*4.f))*3400.f);
 }
 EngineSound->QueueAudio(reinterpret_cast<uint8*>(Samples.GetData()),Samples.Num()*sizeof(int16));
}
