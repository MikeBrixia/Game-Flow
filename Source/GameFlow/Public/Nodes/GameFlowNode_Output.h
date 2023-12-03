// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowNode_Output.generated.h"

/**
 * The exit node of the GameFlow asset. This nodes are used
 * to terminate the execution of a GameFlow asset.
 */
UCLASS()
class GAMEFLOW_API UGameFlowNode_Output final : public UGameFlowNode
{
	GENERATED_BODY()

public:

	UGameFlowNode_Output();
	
	virtual void Execute_Implementation(const FName& PinName) override;
	
};
