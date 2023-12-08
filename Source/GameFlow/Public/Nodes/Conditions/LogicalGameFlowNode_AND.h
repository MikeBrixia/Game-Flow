// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/GameFlowNode.h"
#include "UObject/Object.h"
#include "LogicalGameFlowNode_AND.generated.h"

/**
 * Logical AND boolean operator node.
 */
UCLASS(DisplayName="AND")
class GAMEFLOW_API ULogicalGameFlowNode_AND : public UGameFlowNode
{
	GENERATED_BODY()

public:
	ULogicalGameFlowNode_AND();
	
	virtual void Execute_Implementation(const FName& PinName) override;
	virtual void OnFinishExecute_Implementation() override;
	
private:
	
	/* All the ports which should evaluate to true for the AND operator to execute it's output. */
	UPROPERTY()
	TArray<bool> ConditionsPorts;
};
