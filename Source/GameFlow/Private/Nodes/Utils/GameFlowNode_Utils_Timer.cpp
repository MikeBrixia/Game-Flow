// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/Utils/GameFlowNode_Utils_Timer.h"

#include "GameFlow.h"

UGameFlowNode_Utils_Timer::UGameFlowNode_Utils_Timer()
{
	TypeName = "Latent";

	Time = 2.f;
	StepTime = 0.f;
	
	bLoop = false;
	bStepLoop = false;
	
	AddInputPin("Start");
	AddInputPin("Stop");
	AddInputPin("Skip");
	AddInputPin("Resume");

	AddOutputPin("Completed");
	AddOutputPin("Stopped");
	AddOutputPin("Step");
	AddOutputPin("Skipped");
}

void UGameFlowNode_Utils_Timer::Execute_Implementation(const FName PinName)
{
	Super::Execute_Implementation(PinName);
	
	if(PinName.IsEqual("Start"))
	{
		StartTimer();
	}
	else if(PinName.IsEqual("Stop"))
	{
		PauseTimer();
	}
	else if(PinName.IsEqual("Skip"))
	{
		SkipTimer();
	}
	else if(PinName.IsEqual("Resume"))
	{
		ResumeTimer();
	}
}

void UGameFlowNode_Utils_Timer::StartTimer()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	TimerManager.SetTimer(CompletionTimerHandle, [=]()
			{
		        const float DeltaTime = GetWorld()->DeltaTimeSeconds;
		        TimerUpdate(DeltaTime);
			},GetWorld()->DeltaTimeSeconds, true);
}

void UGameFlowNode_Utils_Timer::SkipTimer()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	TimerManager.ClearTimer(CompletionTimerHandle);
	TriggerOutputPin("Skip");
}

void UGameFlowNode_Utils_Timer::ResumeTimer()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if(TimerManager.IsTimerPaused(CompletionTimerHandle))
	{
		TimerManager.UnPauseTimer(CompletionTimerHandle);
	}
}

void UGameFlowNode_Utils_Timer::PauseTimer()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if(TimerManager.IsTimerActive(CompletionTimerHandle))
	{
		TimerManager.PauseTimer(CompletionTimerHandle);
	}
	
}

void UGameFlowNode_Utils_Timer::TimerUpdate(float DeltaTime)
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	GEngine->AddOnScreenDebugMessage(0, 3.f, FColor::Purple, "Timer update");
	if(CurrentTime >= Time)
	{
		if(bLoop)
		{
			CurrentTime = 0.f;
		}
		else
		{
			TimerManager.ClearTimer(CompletionTimerHandle);
		}
		TriggerOutputPin("Completed");
	}

	if(StepCurrentTime >= StepTime && bCanStep)
	{
		if(bStepLoop)
		{
			StepCurrentTime = 0.f;
		}
       
		bCanStep = bStepLoop;
		TriggerOutputPin("Step");
	}
	
	CurrentTime += DeltaTime;
	StepCurrentTime += DeltaTime;
}
