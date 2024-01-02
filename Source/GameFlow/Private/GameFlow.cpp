// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFlow.h"

#define LOCTEXT_NAMESPACE "FGameFlowModule"
DEFINE_LOG_CATEGORY(LogGameFlow)

void FGameFlowModule::StartupModule()
{
	
}

void FGameFlowModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGameFlowModule, GameFlow)