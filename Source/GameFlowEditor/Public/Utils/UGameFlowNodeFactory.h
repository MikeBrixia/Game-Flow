// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Nodes/GameFlowNode.h"
#include "UObject/Object.h"
#include "UGameFlowNodeFactory.generated.h"

/**
 * Factory used to create Game Flow Nodes assets.
 */
UCLASS()
class GAMEFLOWEDITOR_API UGameFlowNodeFactory final : public UFactory
{
	GENERATED_BODY()
	
public:

	/* Create a brand new Game Flow Node Asset. */
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
		                             UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	
};
