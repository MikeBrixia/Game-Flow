// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowNode_Dummy.generated.h"

/**
 * Dummy nodes are used as a replacement for invalid nodes
 * inside Game Flow Assets. Their goal is to replace the nodes
 * which have been marked as invalid and removed from the asset
 * while also keep the execution flow work.
 */
UCLASS()
class GAMEFLOW_API UGameFlowNode_Dummy : public UGameFlowNode
{
	GENERATED_BODY()

public:

	UGameFlowNode_Dummy();
	virtual void Execute_Implementation(const FName& PinName) override;
};
