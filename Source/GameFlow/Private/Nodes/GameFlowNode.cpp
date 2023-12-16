// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/GameFlowNode.h"
#include "GameFlowAsset.h"

UGameFlowNode::UGameFlowNode()
{
	TypeName = "Event";
}

void UGameFlowNode::Execute_Implementation(const FName& PinName)
{
}

void UGameFlowNode::OnFinishExecute_Implementation()
{
}

void UGameFlowNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	
	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	
	const bool bIsPinContainerProperty = PropertyName.IsEqual("InputPins") || PropertyName.IsEqual("OutputPins");
	// If we've modified an input pin, broadcast pin name changed event.
	if(OnNodePinNameChange.IsBound() && bIsPinContainerProperty)
	{
		OnNodePinNameChange.Broadcast();
	}
	// If typename has been modified, broadcast node typename changed event.
	else if(OnNodeTypeChange.IsBound() && PropertyName.IsEqual("TypeName"))
	{
		OnNodeTypeChange.Broadcast(TypeName);
	}
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
		OnFinishExecute();
	}
	
	// Execute the next node.
	OwnerAsset->AddActiveNode(NextNode);
	NextNode->Execute(OutputPin);
}

#if WITH_EDITOR

FName UGameFlowNode::GenerateAddPinName(uint8 PinDirection)
{
	FString PinName;
	switch(PinDirection)
	{
	default: break;

		// Input case
	case 0:
		PinName = FString::Printf(TEXT("NewPin_%d"), InputPins.Num() + 1);
		break;
		// Output case
	case 1:
		PinName = FString::Printf(TEXT("NewPin_%d"), OutputPins.Num() + 1);
		break;
	}
	return FName(PinName);
}

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
	if(Output != nullptr)
	{
		OutputPins.Add(PinName);
		Outputs.Add(PinName, Output);
	}
}

void UGameFlowNode::RemoveOutput(const FName& PinName)
{
	OutputPins.Remove(PinName);
	Outputs.Remove(PinName);
}

#endif

