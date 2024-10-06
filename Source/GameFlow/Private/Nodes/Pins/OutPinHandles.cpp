// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/Pins/OutPinHandles.h"

#include "GameFlow.h"
#include "Config/GameFlowSettings.h"
#include "Nodes/Pins/InputPinHandle.h"

UOutPinHandle::UOutPinHandle()
{
}

void UOutPinHandle::TriggerPin()
{
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

bool UOutPinHandle::CanCreateConnection(const UPinHandle* OtherPinHandle) const
{
	const bool bIsExecPin = OtherPinHandle->IsA(UInputPinHandle::StaticClass());
	UE_LOG(LogGameFlow, Display, TEXT("%d"), bIsExecPin && Super::CanCreateConnection(OtherPinHandle))
	return bIsExecPin && Super::CanCreateConnection(OtherPinHandle);
}
