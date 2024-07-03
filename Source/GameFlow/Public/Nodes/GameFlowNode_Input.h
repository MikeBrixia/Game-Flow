// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowNode_Input.generated.h"

/**
 * Entry point for the execution of a game flow asset.
 */
UCLASS(DisplayName="Start", NotBlueprintable, NotBlueprintType, Category="I/0")
class GAMEFLOW_API UGameFlowNode_Input final : public UGameFlowNode
{
	GENERATED_BODY()

public:

	UGameFlowNode_Input();

	virtual void Execute_Implementation(const FName PinName) override;
};
