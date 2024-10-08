﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/Pins/InputPinHandle.h"
#include "Nodes/GameFlowNode.h"
#include "Nodes/Pins/OutPinHandles.h"

UInputPinHandle::UInputPinHandle()
{
}

void UInputPinHandle::TriggerPin()
{
	PinOwner->TryExecute(PinName);
}

bool UInputPinHandle::CanCreateConnection(const UPinHandle* OtherPinHandle) const
{
	const bool bIsOutPin = OtherPinHandle->IsA(UOutPinHandle::StaticClass());
	return bIsOutPin && Super::CanCreateConnection(OtherPinHandle);
}
