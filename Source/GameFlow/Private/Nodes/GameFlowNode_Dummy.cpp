// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/GameFlowNode_Dummy.h"
#include "GameFlow.h"
#include "GameFlowAsset.h"
#include "IAssetTools.h"
#include "Config/GameFlowSettings.h"

UGameFlowNode_Dummy::UGameFlowNode_Dummy()
{
	this->bCanAddInputPin = true;
	this->bCanAddOutputPin = true;
}

void UGameFlowNode_Dummy::Execute_Implementation(const FName& PinName)
{
	Super::Execute_Implementation(PinName);
	
    UGameFlowAsset* GameFlowAsset = GetTypedOuter<UGameFlowAsset>();
	// When dummy node gets executed, terminate the execution
	// of the outer asset and throw an error to output log.
	GameFlowAsset->TerminateExecution();
	UE_LOG(LogGameFlow, Error, TEXT("ERROR: %s asset execution was terminated because it tried to execute a dummy node."),
		*GameFlowAsset->GetName())
}

void UGameFlowNode_Dummy::ReplaceDummyNode() const
{
	// Send a notification to listeners and also send them the node type to use as a replacement
	OnReplaceDummyNodeRequest.Broadcast(ReplacedNodeClass);
}
