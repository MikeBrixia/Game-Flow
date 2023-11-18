// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/GameFlowNode.h"
#include "GameFlowAsset.h"

UGameFlowNode::UGameFlowNode()
{
}

void UGameFlowNode::Execute_Implementation()
{
}

void UGameFlowNode::FinishExecute(const FName OutputPin, bool Finish)
{
	// Find and mark as active the next node.
	UGameFlowNode* NextNode = Outputs.FindChecked(OutputPin);
	UGameFlowAsset* OwnerAsset = Cast<UGameFlowAsset>(GetOuter());
	
	// If node has finished executing, remove it from asset active nodes.
	if(Finish && OwnerAsset != nullptr)
	{
		OwnerAsset->RemoveActiveNode(this);
	}
	
	// Execute the next node.
	OwnerAsset->AddActiveNode(NextNode);
	NextNode->Execute();
}
