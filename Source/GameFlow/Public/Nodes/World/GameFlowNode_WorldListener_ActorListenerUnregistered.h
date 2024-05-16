// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowNode_WorldListener.h"
#include "UObject/Object.h"
#include "GameFlowNode_WorldListener_ActorListenerUnregistered.generated.h"

/**
 * Listen for actor listener component un-registration.
 */
UCLASS(NotBlueprintable, NotBlueprintType, DisplayName="On Actor Listener Unregistered", meta=(Category="World"))
class GAMEFLOW_API UGameFlowNode_WorldListener_ActorListenerUnregistered final : public UGameFlowNode_WorldListener
{
	GENERATED_BODY()

	virtual void OnComponentUnregistered_Implementation(UGameFlowListener* ListenerComponent) override;
};
