// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowListener.h"
#include "GameplayTagContainer.h"
#include "Nodes/GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowNode_WorldListener.generated.h"

/**
 * Base class for all nodes which needs to listen for world events.
 */
UCLASS(Abstract)
class GAMEFLOW_API UGameFlowNode_WorldListener : public UGameFlowNode
{
	GENERATED_BODY()

public:
	/** The tags which identifies this node listener. */
	UPROPERTY(EditAnywhere, Category="World Listener")
	FGameplayTagContainer IdentityTags;

	/** The tags of component listeners we want to listen to. */
	UPROPERTY(EditAnywhere, Category="World Listener")
	FGameplayTagContainer ListenerTags;
	
	/** Matching strategy for this node tags. */
	UPROPERTY(EditAnywhere, Category="World Listener")
	EGameplayContainerMatchType MatchingStrategy;

	/** The maximum number of times an event can be triggered. if 0 it means indefinite. */
	UPROPERTY(EditAnywhere, Category="World Listener")
	uint32 Limit;

	/** How many events has this node triggered? */
	UPROPERTY(VisibleAnywhere, Category="World Listener")
	uint32 Count;
	
	UGameFlowNode_WorldListener();

	UFUNCTION(BlueprintCallable, Category="Game Flow|World Listener")
	virtual void Execute_Implementation(const FName& PinName) override;

	UFUNCTION(BlueprintCallable, Category="Game Flow|World Listener")
    virtual void OnFinishExecute_Implementation() override;

protected:
	
	void StartListening();
	void StopListening();
	void TriggerEvent();

	/**
	 * Called when you want this node to start listening to an actor component listener events.
	 * @param ListenerComponent The actor game flow component you want this node to start listening to.
	 */
	UFUNCTION(BlueprintCallable, Category="Game Flow|World Listener")
	virtual void ListenToComponent(UGameFlowListener* ListenerComponent);

	/**
	 * Called when you want this node to stop listening to an an actor component listener events.
	 * @param ListenerComponent The actor game flow component you want to stop listening to.
	 */
	UFUNCTION(BlueprintCallable, Category="Game Flow|World Listener")
	virtual void StopListeningToComponent(UGameFlowListener* ListenerComponent);

	UFUNCTION(BlueprintCallable, Category="Game Flow|World Listener")
	virtual void OnComponentRegistered(UGameFlowListener* ListenerComponent);

	UFUNCTION(BlueprintCallable, Category="Game Flow|World Listener")
	virtual void OnComponentUnregistered(UGameFlowListener* ListenerComponent);

	UFUNCTION(BlueprintCallable, Category="Game Flow|World Listener")
	virtual void OnComponentGameplayTagAdded(UGameFlowListener* ListenerComponent, FGameplayTag AddedTag);

	UFUNCTION(BlueprintCallable, Category="Game Flow|World Listener")
	virtual void OnComponentGameplayTagRemoved(UGameFlowListener* ListenerComponent, FGameplayTag RemovedTag);

private:

	void UpdateComponentListener(UGameFlowListener* ListenerComponent);
	bool DoGameplayTagsMatch(FGameplayTagContainer OtherTags);
};



