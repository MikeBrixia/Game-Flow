// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/Utils/GameFlowNode_Utils_Timer.h"

#include "TimerManager.h"

UGameFlowNode_Utils_Timer::UGameFlowNode_Utils_Timer()
{
#if WITH_EDITOR
	TypeName = "Latent";

	AddInputPin_CDO("Start");
	AddInputPin_CDO("Stop");
	AddInputPin_CDO("Skip");
	AddInputPin_CDO("Resume");

	AddOutputPin_CDO("Completed");
	AddOutputPin_CDO("Stopped");
	AddOutputPin_CDO("Step");
	AddOutputPin_CDO("Skipped");

#endif
	Time = 2.f;
	StepTime = 0.f;
	
	bLoop = false;
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
		StopTimer();
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
	GEngine->AddOnScreenDebugMessage(0, 3.f, FColor::Purple, FString::SanitizeFloat(GetWorld()->DeltaTimeSeconds));
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	TimerManager.SetTimer(CompletionTimerHandle, [this]()
	{
		TriggerOutputPin("Completed");
	},Time, true);

	if(StepTime > 0.f)
	{
		TimerManager.SetTimer(StepTimerHandle, [this]()
		{
			TriggerOutputPin("Step");
		}, StepTime, true);
	}
}

void UGameFlowNode_Utils_Timer::SkipTimer()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	TimerManager.ClearTimer(CompletionTimerHandle);
	TimerManager.ClearTimer(StepTimerHandle);
	TriggerOutputPin("Skipped");
}

void UGameFlowNode_Utils_Timer::ResumeTimer()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	if(TimerManager.IsTimerPaused(CompletionTimerHandle))
	{
		TimerManager.UnPauseTimer(CompletionTimerHandle);
	}

	if(TimerManager.IsTimerPaused(StepTimerHandle))
	{
		TimerManager.UnPauseTimer(StepTimerHandle);
	}

	TriggerOutputPin("Stopped");
}

void UGameFlowNode_Utils_Timer::StopTimer()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if(TimerManager.IsTimerActive(CompletionTimerHandle))
	{
		TimerManager.PauseTimer(CompletionTimerHandle);
	}

	if(TimerManager.IsTimerActive(StepTimerHandle))
	{
		TimerManager.PauseTimer(StepTimerHandle);
	}
}

#if WITH_EDITOR

FString UGameFlowNode_Utils_Timer::GetCustomDebugInfo() const
{
	const UWorld* World =  GetWorld();
	if(World != nullptr && CompletionTimerHandle.IsValid())
	{
		const FTimerManager& TimerManager = World->GetTimerManager();
		return FString::Printf(TEXT("Elapsed time: %f \n"), TimerManager.GetTimerElapsed(CompletionTimerHandle));
	}
	return "";
}

#endif