// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowNode_WorldListener.h"
#include "UObject/Object.h"
#include "GameFlowNode_WorldListener_ActorRegisteredListener.generated.h"

/**
 * Listen for actor component listener registrations
 */
UCLASS(NotBlueprintable, NotBlueprintType, DisplayName="On Actor Listener Registered", meta=(Category="World"))
class GAMEFLOW_API UGameFlowNode_WorldListener_ActorRegisteredListener : public UGameFlowNode_WorldListener
{
	GENERATED_BODY()

public:

	UGameFlowNode_WorldListener_ActorRegisteredListener();
	
	virtual void OnComponentRegistered_Implementation(UGameFlowListener* ListenerComponent) override;
};