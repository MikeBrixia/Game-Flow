// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/GameFlowNode.h"
#include "GameFlowNode_Utils_Timer.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, NotBlueprintType, DisplayName="Timer", meta=(Category="Flow Control"))
class GAMEFLOW_API UGameFlowNode_Utils_Timer : public UGameFlowNode
{
	GENERATED_BODY()

public:

	UGameFlowNode_Utils_Timer();

	/** Amount of time needed to complete the timer. */
	UPROPERTY(EditAnywhere, meta=(GF_Debuggable="enabled"))
	float Time;

	/** Trigger step event each step time interval. */
	UPROPERTY(EditAnywhere, meta=(GF_Debuggable="enabled"))
	float StepTime;

	/** If true, this node will restart itself after completion. */
	UPROPERTY(EditAnywhere)
	bool bLoop;

private:
	
	/** Handle for the currently playing timer. */
	FTimerHandle CompletionTimerHandle;
	
	/** Handle for step time*/
	FTimerHandle StepTimerHandle;
	
	virtual void Execute_Implementation(const FName PinName) override;

	void StartTimer();
	void SkipTimer();
	void ResumeTimer();
	void StopTimer();

#if WITH_EDITOR
    virtual FString GetCustomDebugInfo() const override;
#endif
};
