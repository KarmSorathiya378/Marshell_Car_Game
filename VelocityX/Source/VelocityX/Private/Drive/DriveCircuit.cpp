#include "DriveGame.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

ADriveCircuit::ADriveCircuit()
{
 PrimaryActorTick.bCanEverTick = false;
 RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CircuitRoot"));
}
FVector ADriveCircuit::Point(float A) { return FVector(40000.f * FMath::Cos(A), 24000.f * FMath::Sin(A), 0.f); }
FVector ADriveCircuit::Tangent(float A) { return FVector(-40000.f * FMath::Sin(A), 24000.f * FMath::Cos(A), 0.f).GetSafeNormal(); }
float ADriveCircuit::AngleAt(FVector P) { float A = FMath::Atan2(P.Y / 24000.f, P.X / 40000.f); return A < 0.f ? A + 2.f * PI : A; }
UInstancedStaticMeshComponent* ADriveCircuit::Batch(const TCHAR* Name, const TCHAR* Shape, const TCHAR* Material, bool bCollision)
{
 auto* C = NewObject<UInstancedStaticMeshComponent>(this, Name);
 C->SetupAttachment(RootComponent);
 C->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"), Shape, Shape)));
 C->SetMaterial(0, LoadObject<UMaterialInterface>(nullptr, *FString::Printf(TEXT("/Game/Drive/Materials/M_%s.M_%s"), Material, Material)));
 C->SetCollisionEnabled(bCollision ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
 C->SetCollisionResponseToAllChannels(ECR_Block);
 C->SetCullDistances(45000, 65000);
 C->RegisterComponent();
 Batches.Add(C);
 return C;
}
void ADriveCircuit::Place(UInstancedStaticMeshComponent* B, FVector P, FVector S, FRotator R)
{
 B->AddInstance(FTransform(R, P, S));
}
void ADriveCircuit::BeginPlay()
{
 Super::BeginPlay();
 auto* Ground = Batch(TEXT("Ground"), TEXT("Cube"), TEXT("Sand"), true);
 Place(Ground, FVector(0,0,-140), FVector(1500,1500,2));
 auto* Road = Batch(TEXT("Asphalt"), TEXT("Cube"), TEXT("Asphalt"), true);
 auto* White = Batch(TEXT("Markings"), TEXT("Cube"), TEXT("White"));
 auto* Red = Batch(TEXT("CurbsRed"), TEXT("Cube"), TEXT("Red"));
 auto* Barrier = Batch(TEXT("Barriers"), TEXT("Cube"), TEXT("Concrete"), true);
 auto* Dark = Batch(TEXT("Structures"), TEXT("Cube"), TEXT("Dark"));
 auto* Cyan = Batch(TEXT("TrackAccents"), TEXT("Cube"), TEXT("Cyan"));
 for (int32 I=0; I<320; ++I)
 {
  const float A=I*2.f*PI/320.f, B=(I+1)*2.f*PI/320.f;
  const FVector P=Point(A), Q=Point(B), Mid=(P+Q)*.5f, T=(Q-P).GetSafeNormal(), N(-T.Y,T.X,0);
  const float Len=FVector::Distance(P,Q);
  const FRotator R=T.Rotation();
  Place(Road,Mid+FVector(0,0,-20),FVector((Len+6)/100.f,24,0.4f),R);
  for (float Side : {-1.f,1.f})
  {
   Place(I%2 ? White:Red,Mid+N*Side*1240+FVector(0,0,2),FVector((Len+3)/100.f,1.2f,.08f),R);
   Place(White,Mid+N*Side*1140+FVector(0,0,1),FVector((Len+3)/100.f,.12f,.025f),R);
   Place(Barrier,Mid+N*Side*1450+FVector(0,0,60),FVector((Len+5)/100.f,.65f,1.2f),R);
   if (I%4==0) Place(Cyan,Mid+N*Side*1415+FVector(0,0,112),FVector(Len/100.f,.08f,.12f),R);
  }
  if(I%3==0) Place(White,Mid+FVector(0,0,1),FVector(2.5f,.13f,.025f),R);
 }
 // Start / finish gantry and checkerboard.
 FVector Start=Point(0); FRotator StartR=Tangent(0).Rotation();
 for(int32 I=0;I<12;++I) for(int32 J=0;J<2;++J)
  Place((I+J)%2 ? White:Dark,Start+FVector(-1100+I*200,J*200,2),FVector(2,2,.04f));
 for(float Side : {-1.f,1.f}) Place(Dark,Start+FVector(Side*1390,0,500),FVector(1,1,10));
 Place(Dark,Start+FVector(0,0,990),FVector(29,1.8f,2));
 Place(Cyan,Start+FVector(0,-94,900),FVector(28,.08f,.14f));
 auto* Sign=NewObject<UTextRenderComponent>(this);
 Sign->SetupAttachment(RootComponent); Sign->SetRelativeLocation(Start+FVector(0,-96,970));
 Sign->SetRelativeRotation(FRotator(0,-90,0)); Sign->SetHorizontalAlignment(EHTA_Center);
 Sign->SetText(FText::FromString(TEXT("V E L O C I T Y   /   X"))); Sign->SetWorldSize(105);
 Sign->SetTextRenderColor(FColor(210,245,255)); Sign->RegisterComponent();
 // Deterministic low draw-call desert landscape.
 auto* Rock=Batch(TEXT("Mountains"),TEXT("Cone"),TEXT("Rock"));
 if(auto* Ridge=LoadObject<UStaticMesh>(nullptr,TEXT("/Game/Drive/Meshes/SM_Ridge.SM_Ridge"))) Rock->SetStaticMesh(Ridge);
 auto* Trunk=Batch(TEXT("TreeTrunks"),TEXT("Cylinder"),TEXT("Trunk"));
 auto* Leaves=Batch(TEXT("TreeCrowns"),TEXT("Cone"),TEXT("Leaves"));
 FRandomStream Random(427);
 for(int32 I=0;I<150;++I)
 {
  float A=Random.FRandRange(0,2*PI); FVector P=Point(A); FVector N=FVector(P.X/40000.f,P.Y/24000.f,0).GetSafeNormal();
  P+=N*Random.FRandRange(6500,22000); float H=Random.FRandRange(1800,7500);
  Place(Rock,P+FVector(0,0,H*.35f),FVector(H*.026f,H*.022f,H*.014f),FRotator(0,Random.FRandRange(0,360),0));
 }
 for(int32 I=0;I<350;++I)
 {
  float A=Random.FRandRange(0,2*PI); FVector P=Point(A); FVector N=FVector(P.X/40000.f,P.Y/24000.f,0).GetSafeNormal();
  P+=N*(I%3==0 ? -Random.FRandRange(2400,9000) : Random.FRandRange(2100,6500));
  float S=Random.FRandRange(.8f,1.8f);
  Place(Trunk,P+FVector(0,0,150*S),FVector(.45f*S,.45f*S,3*S));
  Place(Leaves,P+FVector(0,0,400*S),FVector(3*S,3*S,6*S));
  Place(Leaves,P+FVector(0,0,600*S),FVector(2.2f*S,2.2f*S,4*S));
 }
 // Pit architecture inside the opening straight.
 for(int32 I=0;I<10;++I)
 {
  FVector P=Point(.12f+I*.018f); P.X-=3500;
  Place(Dark,P+FVector(0,0,330),FVector(8,7,6.6f));
  Place(Cyan,P+FVector(410,0,480),FVector(.08f,6,.18f));
  Place(White,P+FVector(0,0,690),FVector(9,8,.3f));
 }
 auto* Sun=GetWorld()->SpawnActor<ADirectionalLight>();
 auto* SunC=Cast<UDirectionalLightComponent>(Sun->GetLightComponent());
 SunC->SetMobility(EComponentMobility::Movable);
 Sun->SetActorRotation(FRotator(-28,-42,0));
 SunC->SetIntensity(5.f); SunC->SetLightColor(FLinearColor(1.f,.81f,.62f));
 SunC->bAtmosphereSunLight=true;
 auto* Atmos=GetWorld()->SpawnActor<AActor>();
 auto* AtmosC=NewObject<USkyAtmosphereComponent>(Atmos); Atmos->SetRootComponent(AtmosC); AtmosC->RegisterComponent();
 auto* Sky=GetWorld()->SpawnActor<ASkyLight>();
 Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);
 Sky->GetLightComponent()->SetIntensity(1.1f);
 Sky->GetLightComponent()->RecaptureSky();
 auto* Fog=GetWorld()->SpawnActor<AExponentialHeightFog>();
 Fog->GetComponent()->SetFogDensity(.006f); Fog->GetComponent()->SetFogHeightFalloff(.2f);
 Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(.46f,.55f,.68f));
 auto* PP=GetWorld()->SpawnActor<APostProcessVolume>(); PP->bUnbound=true;
 PP->Settings.bOverride_AutoExposureMethod=true; PP->Settings.AutoExposureMethod=EAutoExposureMethod::AEM_Manual;
 PP->Settings.bOverride_AutoExposureBias=true; PP->Settings.AutoExposureBias=10.f;
 PP->Settings.bOverride_MotionBlurAmount=true; PP->Settings.MotionBlurAmount=.12f;
 PP->Settings.bOverride_BloomIntensity=true; PP->Settings.BloomIntensity=.35f;
 UE_LOG(LogTemp,Display,TEXT("VELOCITY_READY: circuit generated, %d instanced batches"),Batches.Num());
}
