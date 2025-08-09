// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFlowListener.h"
#include "GameFlowSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UGameFlowListener::UGameFlowListener()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}

// Called when the game starts
void UGameFlowListener::BeginPlay()
{
	Super::BeginPlay();
}

void UGameFlowListener::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);
}

// Called every frame
void UGameFlowListener::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGameFlowListener::AddGameplayTag(FGameplayTag GameplayTag)
{
	if(GameplayTag.IsValid())
	{
		const UGameFlowSubsystem* GameFlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameFlowSubsystem>();
		
		IdentityTags.AddTag(GameplayTag);
		GameFlowSubsystem->OnGameplayTagAdded.Broadcast(this, GameplayTag);
	}
}

void UGameFlowListener::RemoveGameplayTag(FGameplayTag GameplayTag)
{
	if(GameplayTag.IsValid())
	{
		const UGameFlowSubsystem* GameFlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameFlowSubsystem>();
		
		IdentityTags.RemoveTag(GameplayTag);
		GameFlowSubsystem->OnGameplayTagRemoved.Broadcast(this, GameplayTag);
	}
}
