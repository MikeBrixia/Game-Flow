// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/Debug/GameFlowNode_Debug_Log.h"
#include "GameFramework/GameSession.h"

UGameFlowNode_Debug_Log::UGameFlowNode_Debug_Log()
{
	AddInputPin("Exec");
	AddOutputPin("Out");
	
	bShouldLogToConsole = false;
	DebugMessage = "Hello!";
	Time = 5.f;
	DebugMessageColor = FColor::Cyan;
	TypeName = "Debug";
}

void UGameFlowNode_Debug_Log::Execute_Implementation(const FName PinName)
{
	Super::Execute_Implementation(PinName);

	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, Time, DebugMessageColor, DebugMessage, true);
	}

	if(bShouldLogToConsole)
	{
		UE_LOG(LogGameSession, Display, TEXT("%s"), *DebugMessage)
	}

	FinishExecute(true);
}

void UGameFlowNode_Debug_Log::OnFinishExecute_Implementation()
{
	Super::OnFinishExecute_Implementation();

	// Execute default output pin.
	TriggerOutputPin("Out");
}
