// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GameFlowListener.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNotifyGameFlowListener);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceiveGameFlowEvent, FGameplayTagContainer, GameplayTags);

UCLASS(ClassGroup=(GameFlow), meta=(BlueprintSpawnableComponent))
class GAMEFLOW_API UGameFlowListener : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGameFlowListener();

	/** Gameplay Tags used to identify this component listener inside game flow. */
	UPROPERTY(EditAnywhere, Category="Game Flow")
	FGameplayTagContainer IdentityTags;

	/** Called when this listener component receives an event notification from a game flow asset. */
	UPROPERTY(BlueprintAssignable, Category="Game Flow")
	FOnReceiveGameFlowEvent OnReceiveGameFlowEvent;

	/** Called when this listener component wants to send an event notification to game flow assets. */
	UPROPERTY(BlueprintCallable, Category="Game Flow")
	FOnNotifyGameFlowListener OnNotifyGameFlowListener;
	
	UFUNCTION(BlueprintCallable, Category="Game Flow")
	void AddGameplayTag(FGameplayTag GameplayTag);

	UFUNCTION(BlueprintCallable, Category="Game Flow")
	void RemoveGameplayTag(FGameplayTag GameplayTag);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void DestroyComponent(bool bPromoteChildren) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
