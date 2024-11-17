// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/GameFlowNode.h"
#include "UObject/Object.h"
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
	UPROPERTY(EditAnywhere)
	float Time;

	/** Trigger step event each step time interval. */
	UPROPERTY(EditAnywhere)
	float StepTime;

	/** If true, this node will restart itself after completion. */
	UPROPERTY(EditAnywhere)
	bool bLoop;

	/** If true, step will be called each time timer wills exceed step time */
	UPROPERTY(EditAnywhere)
	bool bStepLoop;
	
private:

	UPROPERTY(Transient, meta=(GF_Debuggable="enabled"))
	float CurrentTime;

	UPROPERTY(Transient, meta=(GF_Debuggable="enabled"))
	float StepCurrentTime;

	UPROPERTY(Transient, meta=(GF_Debuggable="enabled"))
	bool bCanStep;
	
	/** Handle for the currently playing timer. */
	FTimerHandle CompletionTimerHandle;
	
	virtual void Execute_Implementation(const FName PinName) override;

	void StartTimer();
	void SkipTimer();
	void ResumeTimer();
	void PauseTimer();

	void TimerUpdate(float DeltaTime);
};
