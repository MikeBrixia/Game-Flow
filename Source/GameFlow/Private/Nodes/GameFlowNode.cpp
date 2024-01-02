// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/GameFlowNode.h"
#include "GameFlow.h"
#include "GameFlowAsset.h"

FGameFlowPinNodePair::FGameFlowPinNodePair()
{
	this->InputPinName = EName::None;
	this->Node = nullptr;
}

FGameFlowPinNodePair::FGameFlowPinNodePair(const FName& InputPinName, UGameFlowNode* Node)
{
	this->InputPinName = InputPinName;
	this->Node = Node;
}

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

void UGameFlowNode::FinishExecute(const FName OutputPin, bool bFinish)
{
	// Find and mark as active the next node.
	const auto Pair = GetNextNode(OutputPin);
	UGameFlowNode* NextNode = Pair.Node;
	const FName ConnectionPinName = Pair.InputPinName;
	
	UGameFlowAsset* OwnerAsset = GetTypedOuter<UGameFlowAsset>();
	
	// If node has finished executing, remove it from asset active nodes.
	if(bFinish && OwnerAsset != nullptr)
	{
		OwnerAsset->RemoveActiveNode(this);
		OnFinishExecute();
	}
	
	// Execute the next node.
	OwnerAsset->AddActiveNode(NextNode);
	NextNode->Execute(ConnectionPinName);
}

#if WITH_EDITOR

void UGameFlowNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	
	const FName& PropertyName = PropertyChangedEvent.GetPropertyName();
	
	const int32 Index = PropertyChangedEvent.GetArrayIndex(PropertyName.ToString());
	if(PropertyName.IsEqual("InputPins"))
	{
		TArray<FName> Keys;
		Inputs.GenerateKeyArray(Keys);
		
		const FName InputPinsDiffs = FindPinsDiff(Keys, InputPins)[0];
		const FName NewPinName = InputPins[Index];
		const FGameFlowPinNodePair Pair = Inputs.FindRef(InputPinsDiffs);

		// Update current node outputs.
		Inputs.Remove(InputPinsDiffs);
		Inputs.Add(NewPinName, Pair);
		UGameFlowNode* ConnectedNode = Pair.Node;
		// If the modified pin was connected to another node,
		// update it's inputs.
		if(ConnectedNode != nullptr)
		{
			ConnectedNode->Outputs.Add(Pair.InputPinName, FGameFlowPinNodePair(NewPinName, this));
		}
	}
	else if(PropertyName.IsEqual("OutputPins"))
	{
		TArray<FName> Keys;
		Outputs.GenerateKeyArray(Keys);
		
		const FName OutputPinDiff = FindPinsDiff(Keys, OutputPins)[0];
		const FName NewPinName = OutputPins[Index];
		const FGameFlowPinNodePair Pair = Outputs.FindRef(OutputPinDiff);

		// Update current node outputs.
		Outputs.Remove(OutputPinDiff);
		Outputs.Add(NewPinName, Pair);
		
		UGameFlowNode* ConnectedNode = Pair.Node;
		// If the modified pin was connected to another node,
		// update it's inputs.
		if(ConnectedNode != nullptr)
		{
			ConnectedNode->Inputs.Add(Pair.InputPinName, FGameFlowPinNodePair(NewPinName, this));
		}
	}

	// Notify listeners that this asset has been modified.
	OnEditAsset.Broadcast();
}

TArray<FName> UGameFlowNode::FindPinsDiff(const TArray<FName>& Array0, const TArray<FName>& Array1) const
{
	TArray<FName> Diff = Array0.FilterByPredicate([=] (const FName& PinName)
	{
		return !Array1.Contains(PinName);
	});
	return Diff;
}

#endif

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

void UGameFlowNode::AddInput(const FName PinName, const FGameFlowPinNodePair Input)
{
	// Add input pin if not already present.
	InputPins.AddUnique(PinName);
	
	const bool bValidOutput = Input.Node != nullptr && !Input.InputPinName.IsNone();
	const bool bRecursiveOutput = Input.Node == this;
	// Map pins only if the input is valid and non-recursive, otherwise ignore mapping.
	if(bValidOutput && !bRecursiveOutput)
	{
		Inputs.Add(PinName, Input);
	}
}

void UGameFlowNode::RemoveInputPin(const FName PinName)
{
	InputPins.Remove(PinName);
	Inputs.Remove(PinName);
}

void UGameFlowNode::AddOutput(const FName PinName, const FGameFlowPinNodePair Output)
{
	// Add output pin if not already present.
	OutputPins.AddUnique(PinName);
	
	const bool bValidOutput = Output.Node != nullptr && !Output.InputPinName.IsNone();
	const bool bRecursiveOutput = Output.Node == this;
	// Map pins only if the output is valid and non-recursive, otherwise ignore mapping.
	if(bValidOutput && !bRecursiveOutput)
	{
		Outputs.Add(PinName, Output);
	}
}

void UGameFlowNode::RemoveOutput(const FName PinName)
{
	OutputPins.Remove(PinName);
	Outputs.Remove(PinName);
}


