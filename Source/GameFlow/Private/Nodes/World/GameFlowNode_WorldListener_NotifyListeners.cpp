// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/World/GameFlowNode_WorldListener_NotifyListeners.h"
#include "GameFlowSubsystem.h"
#include "Engine/GameInstance.h"

UGameFlowNode_WorldListener_NotifyListeners::UGameFlowNode_WorldListener_NotifyListeners()
{
#if WITH_EDITOR
	TypeName = "Event";

	AddInputPin_CDO("Exec");
	AddOutputPin_CDO("Out");
#endif
}

void UGameFlowNode_WorldListener_NotifyListeners::Execute_Implementation(const FName PinName)
{
	Super::Execute_Implementation(PinName);

	UGameFlowSubsystem* GameFlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameFlowSubsystem>();
	// Send event notification to all component listener with matching tags and strategy.
	GameFlowSubsystem->NotifyListeners(IdentityTags, MatchingStrategy);

	FinishExecute(true);
}

void UGameFlowNode_WorldListener_NotifyListeners::OnFinishExecute_Implementation()
{
	Super::OnFinishExecute_Implementation();

	// Execute default output pin.
	TriggerOutputPin("Out");
}
