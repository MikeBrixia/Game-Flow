// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowNode_WorldListener.h"
#include "UObject/Object.h"
#include "GameFlowNode_WorldListener_ActorListener.generated.h"

/**
 * Listen for actor lister component notification events.
 */
UCLASS(NotBlueprintable, NotBlueprintType, DisplayName="On Actor Notify", meta=(Category="World"))
class GAMEFLOW_API UGameFlowNode_WorldListener_ActorListener : public UGameFlowNode_WorldListener
{
	GENERATED_BODY()
    
	virtual void ListenToComponent_Implementation(UGameFlowListener* ListenerComponent) override;
	virtual void StopListeningToComponent_Implementation(UGameFlowListener* ListenerComponent) override;
};
