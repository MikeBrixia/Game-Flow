// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowNode_FlowControl_DoN.generated.h"

/**
 * Executes output pin only N times before needing to be reset.
 */
UCLASS(NotBlueprintable, NotBlueprintType, DisplayName="Do N", meta=(Category="Flow Control"))
class GAMEFLOW_API UGameFlowNode_FlowControl_DoN final : public UGameFlowNode
{
	GENERATED_BODY()

public:
	/** The number of times this node output pin can be executed before needing to be reset. */
	UPROPERTY(EditAnywhere, Category="Do N", meta=(GF_Debuggable="enabled"))
	uint32 N;
	
	UGameFlowNode_FlowControl_DoN();
	
	virtual void Execute_Implementation(const FName PinName) override;
    virtual void OnFinishExecute_Implementation() override;
private:
	
	/** The number of times the output pin has been executed. */
	UPROPERTY(meta=(GF_Debuggable="enabled"))
	uint32 Count;
};
