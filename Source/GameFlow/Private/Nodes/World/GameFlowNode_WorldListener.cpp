// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFlow/Public/Nodes/World/GameFlowNode_WorldListener.h"
#include "GameFlowSubsystem.h"

UGameFlowNode_WorldListener::UGameFlowNode_WorldListener()
{
	TypeName = "Event";
	
	AddInputPin("Start");
	AddInputPin("Stop");
	
	AddOutputPin("Trigger Event");
	AddOutputPin("Completed");
	AddOutputPin("Stopped");

	Limit = 0;
	Count = 0;
}

void UGameFlowNode_WorldListener::Execute_Implementation(const FName PinName)
{
	Super::Execute_Implementation(PinName);
	
	if(PinName.IsEqual("Start"))
	{
		StartListening();
	}
	else if(PinName.IsEqual("Stop"))
	{
		StopListening();
	}
}

void UGameFlowNode_WorldListener::OnFinishExecute_Implementation()
{
	UGameFlowSubsystem* GameFlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameFlowSubsystem>();

	// Stop listening to game flow subsystem events.
	GameFlowSubsystem->OnListenerComponentRegistered.RemoveAll(this);
	GameFlowSubsystem->OnListenerComponentUnregistered.RemoveAll(this);
	GameFlowSubsystem->OnGameplayTagAdded.RemoveAll(this);
	GameFlowSubsystem->OnGameplayTagRemoved.RemoveAll(this);

	// Reset counter to 0.
	Count = 0;
}

void UGameFlowNode_WorldListener::StartListening()
{
	UGameFlowSubsystem* GameFlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameFlowSubsystem>();
	TArray<UGameFlowListener*> NodeListeners = GameFlowSubsystem->GetListenersByGameplayTags(ListenerTags, MatchingStrategy);

	// Listen to subsystem events relative to component listeners.
	GameFlowSubsystem->OnListenerComponentRegistered.AddDynamic(this, &UGameFlowNode_WorldListener::OnComponentRegistered);
	GameFlowSubsystem->OnListenerComponentUnregistered.AddDynamic(this, &UGameFlowNode_WorldListener::OnComponentUnregistered);
	GameFlowSubsystem->OnGameplayTagAdded.AddDynamic(this, &UGameFlowNode_WorldListener::OnComponentGameplayTagAdded);
	GameFlowSubsystem->OnGameplayTagRemoved.AddDynamic(this, &UGameFlowNode_WorldListener::OnComponentGameplayTagRemoved);
	
	// Listen to all components matching identity tag of this node.
	for(UGameFlowListener* Listener : NodeListeners)
	{
		ListenToComponent(Listener);
	}
}

void UGameFlowNode_WorldListener::StopListening()
{
	FinishExecute(true);
	ExecuteOutputPin("Stopped");
}

void UGameFlowNode_WorldListener::TryTriggeringEvent()
{
	if(Count < Limit)
	{
		Count++;
		OnTriggerEvent();
	}
	else
	{
		OnCompleted();
	}
}

void UGameFlowNode_WorldListener::OnTriggerEvent_Implementation()
{
	ExecuteOutputPin("Trigger Event");
}

void UGameFlowNode_WorldListener::OnCompleted_Implementation()
{
	FinishExecute(true);
	ExecuteOutputPin("Completed");
}

void UGameFlowNode_WorldListener::ListenToComponent_Implementation(UGameFlowListener* ListenerComponent)
{
	// TODO Subclasses of WorldListener should implement this function...
}

void UGameFlowNode_WorldListener::StopListeningToComponent_Implementation(UGameFlowListener* ListenerComponent)
{
	// TODO Subclasses of WorldListener should implement this function...
}

void UGameFlowNode_WorldListener::OnComponentRegistered_Implementation(UGameFlowListener* ListenerComponent)
{
	// TODO Subclasses of WorldListener should implement this function...
}

void UGameFlowNode_WorldListener::OnComponentUnregistered_Implementation(UGameFlowListener* ListenerComponent)
{
	// TODO Subclasses of WorldListener should implement this function...
}

void UGameFlowNode_WorldListener::OnComponentGameplayTagAdded_Implementation(UGameFlowListener* ListenerComponent,
   FGameplayTag AddedTag)
{
	// We only care if the tag which was added is part of the identity of this node.
	if(IdentityTags.HasTag(AddedTag))
	{
		UpdateComponentListener(ListenerComponent);
	}
}

void UGameFlowNode_WorldListener::OnComponentGameplayTagRemoved_Implementation(UGameFlowListener* ListenerComponent,
	FGameplayTag RemovedTag)
{
	// We only care if the tag which was removed is part of the identity of this node.
	if(IdentityTags.HasTag(RemovedTag))
	{
		UpdateComponentListener(ListenerComponent);
	}
}

void UGameFlowNode_WorldListener::UpdateComponentListener(UGameFlowListener* ListenerComponent)
{
	if(DoGameplayTagsMatch(ListenerComponent->IdentityTags))
	{
		ListenToComponent(ListenerComponent);
	}
	else
	{
		StopListeningToComponent(ListenerComponent);
	}
}

bool UGameFlowNode_WorldListener::DoGameplayTagsMatch(FGameplayTagContainer OtherTags)
{
	return MatchingStrategy == EGameplayContainerMatchType::All?
		IdentityTags.HasAllExact(OtherTags)
		: IdentityTags.HasAnyExact(OtherTags);
}
