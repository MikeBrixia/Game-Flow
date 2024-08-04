// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/World/GameFlowNode_WorldListener_ActorListenerUnregistered.h"

void UGameFlowNode_WorldListener_ActorListenerUnregistered::OnComponentUnregistered_Implementation(
	UGameFlowListener* ListenerComponent)
{
	Super::OnComponentUnregistered_Implementation(ListenerComponent);
	TryTriggeringEvent(ListenerComponent->IdentityTags);
}
