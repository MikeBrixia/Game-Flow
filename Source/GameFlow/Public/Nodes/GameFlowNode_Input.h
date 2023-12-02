// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowNode_Input.generated.h"

/**
 * GameFlow input are nodes which represents entry points
 * inside the GameFlow graph. This type of node doesn't have any input
 * pins, only single outputs.
 */
UCLASS()
class GAMEFLOW_API UGameFlowNode_Input final : public UGameFlowNode
{
	GENERATED_BODY()

protected:

	/* The child of the input node. */
	UPROPERTY()
	TObjectPtr<UGameFlowNode> OutputNode;

public:
	
	FORCEINLINE virtual TArray<FName> GetOutputPins() const override { return {"Exec"}; }
	FORCEINLINE virtual UGameFlowNode* GetNextNode(FName PinName) const override { return OutputNode; }
	
	virtual void AddOutput(const FName& PinName, UGameFlowNode* Output) override;
	virtual void RemoveOutput(const FName& PinName) override;
};
