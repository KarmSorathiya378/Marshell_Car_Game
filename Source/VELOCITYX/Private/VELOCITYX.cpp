// Copyright VELOCITY X Racing. All Rights Reserved.

#include "VELOCITYX.h"
#include "Modules/ModuleManager.h"

void FVELOCITYXModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin or .uproject file
}

void FVELOCITYXModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module. For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

IMPLEMENT_PRIMARY_GAME_MODULE(FVELOCITYXModule, VELOCITYX, "VELOCITYX");
