// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/GameFlowNode.h"
#include "GameFlowNode_FlowControl_Sequence.generated.h"

/**
 * Execute many tasks in order.
 */
UCLASS(NotBlueprintable, NotBlueprintType, DisplayName="Sequence", meta=(Category="Flow Control"))
class GAMEFLOW_API UGameFlowNode_FlowControl_Sequence : public UGameFlowNode
{
	GENERATED_BODY()

public:

	UGameFlowNode_FlowControl_Sequence();

	virtual void Execute_Implementation(const FName PinName) override;
};
