// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/GameFlowNode.h"
#include "GameFlowAsset.h"

UGameFlowNode::UGameFlowNode()
{
}

void UGameFlowNode::Execute_Implementation(const FName& PinName)
{
}

void UGameFlowNode::AddOutput(const FName& PinName, UGameFlowNode* Output)
{
	UE_LOG(LogTemp, Warning, TEXT("The node of type: %s cannot have any output nodes, skipping instruction."), *StaticClass()->GetName());
}

void UGameFlowNode::RemoveOutput(const FName& PinName)
{
	UE_LOG(LogTemp, Warning, TEXT("The node of type: %s cannot have any output nodes, skipping instruction."), *StaticClass()->GetName());
}

void UGameFlowNode::FinishExecute(const FName OutputPin, bool bFinish)
{
	// Find and mark as active the next node.
	UGameFlowNode* NextNode = GetNextNode(OutputPin);
	UGameFlowAsset* OwnerAsset = Cast<UGameFlowAsset>(GetOuter());
	
	// If node has finished executing, remove it from asset active nodes.
	if(bFinish && OwnerAsset != nullptr)
	{
		OwnerAsset->RemoveActiveNode(this);
	}
	
	// Execute the next node.
	OwnerAsset->AddActiveNode(NextNode);
	NextNode->Execute(OutputPin);
}
