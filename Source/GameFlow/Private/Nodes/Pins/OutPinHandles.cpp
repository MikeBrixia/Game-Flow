// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/Pins/OutPinHandles.h"
#include "Config/GameFlowSettings.h"
#include "Nodes/Pins/InputPinHandle.h"

UOutPinHandle::UOutPinHandle()
{
}

void UOutPinHandle::TriggerPin()
{
	Super::TriggerPin();
	
	// Trigger all connected exec pins.
	for(const auto& Pin : GetConnections())
	{
		Pin->TriggerPin();
	}

#if WITH_EDITOR
	bIsActive = true;
	ActivatedElapsedTime = UGameFlowSettings::Get()->WireHighlightDuration;
#endif
}

#if WITH_EDITOR

bool UOutPinHandle::CanCreateConnection(const UPinHandle* OtherPinHandle) const
{
	const bool bIsExecPin = OtherPinHandle->IsA(UInputPinHandle::StaticClass());
	return bIsExecPin && Super::CanCreateConnection(OtherPinHandle);
}

#endif
