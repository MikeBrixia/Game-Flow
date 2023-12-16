// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowOperator_OR.generated.h"

/**
 * Logical OR operator as a GameFlow node
 */
UCLASS(DisplayName="OR")
class GAMEFLOW_API UGameFlowOperator_OR : public UGameFlowNode
{
	GENERATED_BODY()

public:

	UGameFlowOperator_OR();

	virtual void Execute_Implementation(const FName& PinName) override;
};
