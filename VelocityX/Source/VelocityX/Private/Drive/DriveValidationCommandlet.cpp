#include "DriveValidationCommandlet.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

UDriveValidationCommandlet::UDriveValidationCommandlet()
{
 IsClient=false; IsServer=false; IsEditor=true; LogToConsole=true;
}
int32 UDriveValidationCommandlet::Main(const FString& Params)
{
#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
 int32 Errors=0; FString Report;
 for(const TCHAR* Name : {TEXT("FDriveRaceTest"),TEXT("FDriveDynamicsTest")})
 {
  auto& Framework=FAutomationTestFramework::Get(); Framework.StartTestByName(Name,0);
  if(!GIsAutomationTesting) { ++Errors; Report+=FString(Name)+TEXT(": NOT REGISTERED\n"); continue; }
  FAutomationTestExecutionInfo Info; const bool bPassed=Framework.StopTest(Info);
  Errors+=Info.GetErrorTotal()+(bPassed?0:1);
  const FString Line=FString::Printf(TEXT("%s: %s (%d errors)\n"),Name,bPassed&&Info.GetErrorTotal()==0?TEXT("PASS"):TEXT("FAIL"),Info.GetErrorTotal());
  Report+=Line; UE_LOG(LogTemp,Display,TEXT("DRIVE_VALIDATION %s"),*Line);
 }
 FFileHelper::SaveStringToFile(Report,*(FPaths::ProjectSavedDir()/TEXT("DriveValidation.txt")));
 return Errors==0?0:1;
#else
 UE_LOG(LogTemp,Error,TEXT("DriveValidation requires a Development Editor build.")); return 1;
#endif
}
