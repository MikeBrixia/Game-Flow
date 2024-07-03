// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowNode_WorldListener.h"
#include "UObject/Object.h"
#include "GameFlowNode_WorldListener_NotifyListeners.generated.h"

/**
 * Send a game flow event to all listeners with matching identity tag inside the world.
 */
UCLASS(NotBlueprintable, NotBlueprintType, DisplayName="Notify Listeners", meta=(Category="World"))
class GAMEFLOW_API UGameFlowNode_WorldListener_NotifyListeners final : public UGameFlowNode
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="Notify Listeners")
	FGameplayTagContainer IdentityTags;

	UPROPERTY(EditAnywhere, Category="Notify Listeners")
	EGameplayContainerMatchType MatchingStrategy;
	
	UGameFlowNode_WorldListener_NotifyListeners();
    
	virtual void Execute_Implementation(const FName PinName) override;
};
