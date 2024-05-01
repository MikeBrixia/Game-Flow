// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowNode_LogicalOperator_OR.generated.h"

/**
 * Game Flow OR logical operator
 */
UCLASS(NotBlueprintable, NotBlueprintType, DisplayName="OR", meta=(Category="Logical operators"))
class GAMEFLOW_API UGameFlowNode_LogicalOperator_OR : public UGameFlowNode
{
	GENERATED_BODY()

public:

	UGameFlowNode_LogicalOperator_OR();

	virtual void Execute_Implementation(const FName& PinName) override;
};
