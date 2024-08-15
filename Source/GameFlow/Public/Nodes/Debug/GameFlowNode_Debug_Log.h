// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowNode_Debug_Log.generated.h"

/**
 * Log a message to the Unreal console
 */
UCLASS(NotBlueprintable, NotBlueprintType, DisplayName="Log", meta=(Category="Debug"))
class GAMEFLOW_API UGameFlowNode_Debug_Log final : public UGameFlowNode
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="Log")
	FString DebugMessage;
	
	UPROPERTY(EditAnywhere, Category="Console")
	bool bShouldLogToConsole;
	
	UPROPERTY(EditAnywhere, Category="Screen")
	FColor DebugMessageColor;
	
	UPROPERTY(EditAnywhere, Category="Screen")
	float Time;

	UGameFlowNode_Debug_Log();
	
	virtual void Execute_Implementation(const FName PinName) override;
	virtual void OnFinishExecute_Implementation() override;
};
