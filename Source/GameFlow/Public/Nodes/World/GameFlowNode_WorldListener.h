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
UCLASS(Abstract, Blueprintable, BlueprintType)
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
	
	virtual void Execute_Implementation(const FName& PinName) override;
    virtual void OnFinishExecute_Implementation() override;

	void TryTriggeringEvent();
	
protected:
	
	void StartListening();
	void StopListening();

	UFUNCTION(BlueprintNativeEvent, Category="Game Flow|World Listener")
	void OnTriggerEvent();
	virtual void OnTriggerEvent_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category="Game Flow|World Listener")
	void OnCompleted();
	virtual void OnCompleted_Implementation();
	
	/**
	 * Called when you want this node to start listening to an actor component listener events.
	 * @param ListenerComponent The actor game flow component you want this node to start listening to.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow|World Listener")
	void ListenToComponent(UGameFlowListener* ListenerComponent);
	virtual void ListenToComponent_Implementation(UGameFlowListener* ListenerComponent);
	
	/**
	 * Called when you want this node to stop listening to an an actor component listener events.
	 * @param ListenerComponent The actor game flow component you want to stop listening to.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow|World Listener")
	void StopListeningToComponent(UGameFlowListener* ListenerComponent);
	virtual void StopListeningToComponent_Implementation(UGameFlowListener* ListenerComponent);
	
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow|World Listener")
	void OnComponentRegistered(UGameFlowListener* ListenerComponent);
	virtual void OnComponentRegistered_Implementation(UGameFlowListener* ListenerComponent);
	
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow|World Listener")
	void OnComponentUnregistered(UGameFlowListener* ListenerComponent);
	virtual void OnComponentUnregistered_Implementation(UGameFlowListener* ListenerComponent);
	
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow|World Listener")
	void OnComponentGameplayTagAdded(UGameFlowListener* ListenerComponent, FGameplayTag AddedTag);
	virtual void OnComponentGameplayTagAdded_Implementation(UGameFlowListener* ListenerComponent, FGameplayTag AddedTag);
	
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow|World Listener")
	void OnComponentGameplayTagRemoved(UGameFlowListener* ListenerComponent, FGameplayTag RemovedTag);
	virtual void OnComponentGameplayTagRemoved_Implementation(UGameFlowListener* ListenerComponent, FGameplayTag RemovedTag);

	UFUNCTION(BlueprintCallable, Category="Game Flow|World Listener")
	bool DoGameplayTagsMatch(FGameplayTagContainer OtherTags);
	
private:

	void UpdateComponentListener(UGameFlowListener* ListenerComponent);
};



