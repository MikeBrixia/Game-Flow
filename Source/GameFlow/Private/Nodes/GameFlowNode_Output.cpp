// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/GameFlowNode_Output.h"
#include "GameFlowAsset.h"

UGameFlowNode_Output::UGameFlowNode_Output()
{
	TypeName = "Output";
	AddInput("Exec", {});
}

void UGameFlowNode_Output::Execute_Implementation(const FName& PinName)
{
	Super::Execute_Implementation(PinName);
    
	// Terminate the execution of the parent game flow asset.
	UGameFlowAsset* GameFlowAsset = CastChecked<UGameFlowAsset>(GetOuter());
	GameFlowAsset->TerminateExecution();
}
