// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowNode_LogicalOperator_AND.generated.h"

/**
 * Game Flow AND logical operator.
 */
UCLASS(NotBlueprintable, NotBlueprintType, DisplayName="AND", meta=(Category="Flow Control"))
class GAMEFLOW_API UGameFlowNode_LogicalOperator_AND final : public UGameFlowNode
{
	GENERATED_BODY()

private:
	/** All the ports which should evaluate to true for the AND operator to execute it's output. */
	UPROPERTY()
	TArray<bool> ConditionalPorts;

	/** The number of ports which are currently evaluated to true. */
	UPROPERTY()
	int TruePortsNum;

public:
	UGameFlowNode_LogicalOperator_AND();
	virtual void Execute_Implementation(const FName PinName) override;

private:
	void Reset();
};
