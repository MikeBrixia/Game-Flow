// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/World/GameFlowNode_WorldListener_ActorRegisteredListener.h"

UGameFlowNode_WorldListener_ActorRegisteredListener::UGameFlowNode_WorldListener_ActorRegisteredListener() : UGameFlowNode_WorldListener()
{
}

void UGameFlowNode_WorldListener_ActorRegisteredListener::OnComponentRegistered_Implementation(
	UGameFlowListener* ListenerComponent)
{
	Super::OnComponentRegistered_Implementation(ListenerComponent);

	// Trigger an event only if the registered component identity tags matches this node
	// identity tags.
	if(DoGameplayTagsMatch(ListenerComponent->IdentityTags))
	{
		TryTriggeringEvent();
	}
}



