// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/PinHandle.h"
#include "UObject/Object.h"
#include "InputPinHandle.generated.h"

/**
 * Pin handles for input pins. Exec pin handles are responsible
 * for executing their parent node from a specific entry point.
 */
UCLASS()
class GAMEFLOW_API UInputPinHandle : public UPinHandle
{
	GENERATED_BODY()

public:
	UInputPinHandle();
	
	virtual void TriggerPin() override;

#if WITH_EDITOR
	virtual bool CanCreateConnection(const UPinHandle* OtherPinHandle) const override;
#endif
};
