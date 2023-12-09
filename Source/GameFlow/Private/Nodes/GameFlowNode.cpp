// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/GameFlowNode.h"
#include "GameFlowAsset.h"

UGameFlowNode::UGameFlowNode()
{
}

void UGameFlowNode::Execute_Implementation(const FName& PinName)
{
}

void UGameFlowNode::OnFinishExecute_Implementation()
{
}

#if WITH_EDITORONLY_DATA
void UGameFlowNode::AddInput(const FName PinName)
{
	// Do we have the permission to add new input pins to this node?
	if(CanAddInputPin() && !PinName.IsEqual(EName::None))
	{
		InputPins.Add(PinName);
	}
}

void UGameFlowNode::RemoveInputPin(const FName PinName)
{
	InputPins.Remove(PinName);
}

void UGameFlowNode::AddOutput(const FName& PinName, UGameFlowNode* Output)
{
	// Do we have the permission to add output pins to this node?
	if(CanAddOutputPin() && !PinName.IsEqual("None"))
	{
		OutputPins.Add(PinName);
		Outputs.Add(PinName);
	}
}

void UGameFlowNode::RemoveOutput(const FName& PinName)
{
	OutputPins.Remove(PinName);
	Outputs.Remove(PinName);
}

#endif

void UGameFlowNode::FinishExecute(const FName OutputPin, bool bFinish)
{
	// Find and mark as active the next node.
	UGameFlowNode* NextNode = GetNextNode(OutputPin);
	UGameFlowAsset* OwnerAsset = Cast<UGameFlowAsset>(GetOuter());
	
	// If node has finished executing, remove it from asset active nodes.
	if(bFinish && OwnerAsset != nullptr)
	{
		OwnerAsset->RemoveActiveNode(this);
		OnFinishExecute();
	}
	
	// Execute the next node.
	OwnerAsset->AddActiveNode(NextNode);
	NextNode->Execute(OutputPin);
}
