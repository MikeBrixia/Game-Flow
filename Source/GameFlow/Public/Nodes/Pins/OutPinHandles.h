// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/PinHandle.h"
#include "UObject/Object.h"
#include "OutPinHandles.generated.h"

/**
 * Pin handles for output pins. Out pin handles are responsible
 * of triggering execution of all the connected exec pins.
 */
UCLASS()
class GAMEFLOW_API UOutPinHandle : public UPinHandle
{
	GENERATED_BODY()

public:
	UOutPinHandle();
	
	virtual void TriggerPin() override;

#if WITH_EDITOR
    virtual bool CanCreateConnection(const UPinHandle* OtherPinHandle) const override;
#endif
};
