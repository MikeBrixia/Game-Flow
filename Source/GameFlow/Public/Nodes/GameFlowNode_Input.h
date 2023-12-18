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
UCLASS(DisplayName="Start", NotBlueprintable, NotBlueprintType)
class GAMEFLOW_API UGameFlowNode_Input final : public UGameFlowNode
{
	GENERATED_BODY()

public:

	UGameFlowNode_Input();
};
