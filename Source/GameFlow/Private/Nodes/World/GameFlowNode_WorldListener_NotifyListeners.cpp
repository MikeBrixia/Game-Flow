// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/World/GameFlowNode_WorldListener_NotifyListeners.h"
#include "GameFlowSubsystem.h"

UGameFlowNode_WorldListener_NotifyListeners::UGameFlowNode_WorldListener_NotifyListeners()
{
	TypeName = "Event";

	AddInputPin("Exec");
	AddOutputPin("Out");
}

void UGameFlowNode_WorldListener_NotifyListeners::Execute_Implementation(const FName& PinName)
{
	Super::Execute_Implementation(PinName);

	UGameFlowSubsystem* GameFlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameFlowSubsystem>();
	// Send event notification to all component listener with matching tags and strategy.
	GameFlowSubsystem->NotifyListeners(IdentityTags, MatchingStrategy);

	FinishExecute(true);
}
