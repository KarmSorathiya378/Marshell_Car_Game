#include "DriveGame.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

namespace { const FLinearColor Ice(.83f,.91f,.96f,1), Muted(.4f,.54f,.62f,1), Cyan(.13f,.95f,.82f,1), Ink(.012f,.023f,.035f,.88f);
FString TimeLabel(float T) { return T<=0 ? TEXT("--:--.---") : FString::Printf(TEXT("%02d:%06.3f"),int32(T)/60,FMath::Fmod(T,60.f)); } }
void ADriveHUD::Label(const FString& T,float X,float Y,float Size,FLinearColor Color) { DrawText(T,Color,X*Unit,Y*Unit,GEngine->GetMediumFont(),Size*Unit*1.4f,false); }
void ADriveHUD::Panel(float X,float Y,float W,float H,FLinearColor Color) { DrawRect(Color,X*Unit,Y*Unit,W*Unit,H*Unit); }
void ADriveHUD::DrawHUD()
{
 Super::DrawHUD(); auto* Car=Cast<ADriveCar>(GetOwningPawn()); if(!Car || !Canvas) return;
 Unit=FMath::Min(Canvas->SizeX/1600.f,Canvas->SizeY/900.f); const float W=Canvas->SizeX/Unit,H=Canvas->SizeY/Unit;
 Panel(32,30,268,72,Ink); Panel(32,30,4,72,Cyan);
 Label(TEXT("VELOCITY / X"),51,43,1.35f,Ice); Label(TEXT("SUNRIDGE  /  CIRCUIT 01"),52,78,.65f,Muted);
 Panel(W-326,30,294,126,Ink); Label(TEXT("LAP"),W-304,45,.7f,Muted);
 Label(FString::Printf(TEXT("%d / 3"),Car->Lap),W-304,65,1.35f,Ice);
 Label(TEXT("CURRENT"),W-194,44,.65f,Muted); Label(TimeLabel(Car->LapTime),W-194,65,.95f,Ice);
 Label(TEXT("BEST"),W-304,120,.65f,Muted); Label(TimeLabel(Car->BestLap),W-214,116,.9f,Cyan);
 Panel(W-326,H-232,294,198,Ink);
 Label(Car->CarName(),W-303,H-213,.75f,Muted);
 Label(FString::Printf(TEXT("%03d"),FMath::RoundToInt(FMath::Abs(Car->Speed)*.036f)),W-307,H-190,3.4f,Ice);
 Label(TEXT("KM/H"),W-110,H-147,.8f,Muted);
 Label(Car->Speed < -10 ? TEXT("R") : FString::FromInt(FMath::Clamp(int32(FMath::Abs(Car->Speed)/1800)+1,1,6)),W-95,H-208,1.6f,Cyan);
 for(int32 I=0;I<24;++I) Panel(W-303+I*10.7f,H-105,7,5,I<FMath::Fmod(FMath::Abs(Car->Speed),1800.f)/75.f?Cyan:FLinearColor(.1f,.15f,.19f,1));
 Label(TEXT("BOOST / SHIFT"),W-303,H-82,.65f,Muted);
 Panel(W-303,H-54,250,5,FLinearColor(.09f,.16f,.2f,1)); Panel(W-303,H-54,250*Car->Boost,5,Cyan);
 // Route overview with live car and next checkpoint.
 Panel(32,H-244,252,210,Ink); Label(TEXT("TRACK POSITION"),51,H-229,.65f,Muted);
 for(int32 I=0;I<80;++I)
 {
  float A=I*2.f*PI/80.f,B=(I+1)*2.f*PI/80.f;
  DrawLine((158+94*FMath::Cos(A))*Unit,(H-130+60*FMath::Sin(A))*Unit,(158+94*FMath::Cos(B))*Unit,(H-130+60*FMath::Sin(B))*Unit,FLinearColor(.25f,.36f,.4f,1),3*Unit);
 }
 FVector P=Car->GetActorLocation(); Panel(154+P.X/40000.f*94,H-134+P.Y/24000.f*60,8,8,Cyan);
 float Gate=(Car->NextGate%16)*2.f*PI/16.f; Panel(155+94*FMath::Cos(Gate),H-133+60*FMath::Sin(Gate),6,6,FLinearColor(1,.65f,.25f,1));
 Label(FString::Printf(TEXT("CHECKPOINT  %02d / 16"),Car->NextGate),51,H-64,.65f,Muted);
 Label(TEXT("WASD  Drive     SPACE  Handbrake     SHIFT  Boost     C  Camera     R  Recover"),320,H-61,.7f,Ice);
 Label(TEXT("ESC  Pause     F5  Restart     F1  Graphics     Gamepad supported"),320,H-38,.65f,Muted);
 if(Car->NoticeTime>0) { Panel(W*.5f-240,136,480,50,Ink); Label(Car->Notice,W*.5f-217,152,.9f,Cyan); }
 if(Car->bOffroad && Car->bStarted) Label(TEXT("OFF TRACK / GRIP REDUCED"),W*.5f-150,H-120,.9f,FLinearColor(1,.6f,.2f,1));
 if(Car->bStarted && Car->Countdown>0)
 {
  Panel(W*.5f-60,H*.42f-10,120,110,Ink); Label(FString::FromInt(FMath::CeilToInt(Car->Countdown)),W*.5f-28,H*.42f,4.f,Cyan);
 }
 if(!Car->bStarted || Car->bFinished || UGameplayStatics::IsGamePaused(GetWorld()))
 {
  Panel(0,0,W,H,FLinearColor(.008f,.016f,.025f,.62f));
  const float X=W*.5f-330,Y=H*.5f-220;
  Panel(X,Y,660,420,FLinearColor(.017f,.032f,.047f,.97f)); Panel(X,Y,660,3,Cyan);
  Label(TEXT("V E L O C I T Y   /   X"),X+42,Y+34,1.8f,Ice);
  Label(TEXT("SUNRIDGE MOTOR PARK  /  TIME ATTACK"),X+43,Y+89,.75f,Muted);
  if(Car->bFinished)
  {
   Label(TEXT("SESSION COMPLETE"),X+43,Y+143,1.45f,Cyan);
   Label(TEXT("3 LAPS   /   ")+TimeLabel(Car->RaceTime),X+43,Y+193,1.1f,Ice);
   Label(TEXT("BEST LAP   ")+TimeLabel(Car->BestLap),X+43,Y+235,1.f,Muted);
   Label(TEXT("ENTER  /  RACE AGAIN"),X+43,Y+334,1.15f,Cyan);
  }
  else if(UGameplayStatics::IsGamePaused(GetWorld()))
  {
   Label(TEXT("TAKE A BREATHER"),X+43,Y+146,1.6f,Ice);
   Label(TEXT("ESC  Resume     F5  New session"),X+43,Y+218,1.f,Muted);
   Label(TEXT("F1  Cycle Performance / Balanced / High"),X+43,Y+268,.9f,Muted);
   Label(TEXT("ALT + F4  Quit"),X+43,Y+334,.9f,Muted);
  }
  else
  {
   Label(TEXT("THREE LAPS. FIND YOUR LIMIT."),X+43,Y+139,1.3f,Ice);
   const TCHAR* Names[]={TEXT("1  APEX GT"),TEXT("2  VELOCE R"),TEXT("3  TITAN X")};
   const TCHAR* Specs[]={TEXT("BALANCED"),TEXT("TRACK GRIP"),TEXT("TOP SPEED")};
   for(int32 I=0;I<3;++I)
   {
    float CX=X+42+I*194;
    Panel(CX,Y+205,181,85,Car->CarType==I?FLinearColor(.05f,.24f,.24f,1):FLinearColor(.035f,.06f,.08f,1));
    Label(Names[I],CX+13,Y+222,.9f,Car->CarType==I?Cyan:Ice);
    Label(Specs[I],CX+13,Y+260,.65f,Muted);
   }
   Label(TEXT("ENTER  /  START YOUR ENGINE"),X+43,Y+330,1.15f,Cyan);
   Label(TEXT("Follow the next checkpoint marker around the circuit."),X+43,Y+375,.65f,Muted);
  }
 }
}
ADriveGameMode::ADriveGameMode() { DefaultPawnClass=ADriveCar::StaticClass(); HUDClass=ADriveHUD::StaticClass(); }
void ADriveGameMode::InitGame(const FString& MapName,const FString& Options,FString& Error)
{
 Super::InitGame(MapName,Options,Error);
 GetWorld()->SpawnActor<APlayerStart>(ADriveCircuit::Point(0)+FVector(0,0,57),ADriveCircuit::Tangent(0).Rotation());
 GetWorld()->SpawnActor<ADriveCircuit>();
}
void ADriveGameMode::RestartPlayer(AController* Player)
{
 FTransform Spawn(ADriveCircuit::Tangent(0).Rotation(),ADriveCircuit::Point(0)+FVector(0,0,57));
 RestartPlayerAtTransform(Player,Spawn);
}
