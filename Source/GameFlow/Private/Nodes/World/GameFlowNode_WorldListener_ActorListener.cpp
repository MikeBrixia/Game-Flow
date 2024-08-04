// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/World/GameFlowNode_WorldListener_ActorListener.h"
#include "GameFlow.h"

void UGameFlowNode_WorldListener_ActorListener::ListenToComponent_Implementation(UGameFlowListener* ListenerComponent)
{
	ListenerComponent->OnNotifyGameFlowListener.AddDynamic(this, &UGameFlowNode_WorldListener::TryTriggeringEvent);
	UE_LOG(LogGameFlow, Display, TEXT("Listening finalised"))
}

void UGameFlowNode_WorldListener_ActorListener::StopListeningToComponent_Implementation(UGameFlowListener* ListenerComponent)
{
	ListenerComponent->OnNotifyGameFlowListener.RemoveAll(this);
}

