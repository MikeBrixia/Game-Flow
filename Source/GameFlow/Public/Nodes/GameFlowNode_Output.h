// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowNode_Output.generated.h"

/**
 * Terminates the execution of a game flow asset
 */
UCLASS(DisplayName="Finish", NotBlueprintable, NotBlueprintType, Category="I/0")
class GAMEFLOW_API UGameFlowNode_Output final : public UGameFlowNode
{
	GENERATED_BODY()

public:

	UGameFlowNode_Output();
	
	virtual void Execute_Implementation(const FName& PinName) override;
	
};
