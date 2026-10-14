// Copyright VELOCITY X Racing. All Rights Reserved.

#include "VelocityInputConfig.h"

// Define input action names
const FName FVelocityInputNames::Throttle          = TEXT("Throttle");
const FName FVelocityInputNames::Brake             = TEXT("Brake");
const FName FVelocityInputNames::SteerLeft         = TEXT("SteerLeft");
const FName FVelocityInputNames::SteerRight        = TEXT("SteerRight");
const FName FVelocityInputNames::Handbrake         = TEXT("Handbrake");
const FName FVelocityInputNames::CycleCamera       = TEXT("Camera");
const FName FVelocityInputNames::ToggleHeadlights  = TEXT("Headlights");
const FName FVelocityInputNames::ResetCar          = TEXT("Reset");
const FName FVelocityInputNames::Pause             = TEXT("Pause");
const FName FVelocityInputNames::MouseSteer        = TEXT("MouseSteer");
const FName FVelocityInputNames::ShiftUp           = TEXT("ShiftUp");
const FName FVelocityInputNames::ShiftDown         = TEXT("ShiftDown");

const FName FVelocityInputNames::IA_Throttle          = TEXT("IA_Throttle");
const FName FVelocityInputNames::IA_Brake             = TEXT("IA_Brake");
const FName FVelocityInputNames::IA_Steer             = TEXT("IA_Steer");
const FName FVelocityInputNames::IA_Handbrake         = TEXT("IA_Handbrake");
const FName FVelocityInputNames::IA_CycleCamera       = TEXT("IA_CycleCamera");
const FName FVelocityInputNames::IA_ToggleHeadlights  = TEXT("IA_ToggleHeadlights");
const FName FVelocityInputNames::IA_ResetCar          = TEXT("IA_ResetCar");
const FName FVelocityInputNames::IA_Pause             = TEXT("IA_Pause");
const FName FVelocityInputNames::IA_MouseSteer        = TEXT("IA_MouseSteer");
const FName FVelocityInputNames::IA_LookAround        = TEXT("IA_LookAround");
const FName FVelocityInputNames::IA_ShiftUp           = TEXT("IA_ShiftUp");
const FName FVelocityInputNames::IA_ShiftDown         = TEXT("IA_ShiftDown");

UVelocityInputConfig::UVelocityInputConfig()
{
	DrivingContextPriority = 0;
	MenuContextPriority = 10;
	PauseContextPriority = 20;
}
