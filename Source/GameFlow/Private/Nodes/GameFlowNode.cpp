﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/GameFlowNode.h"

#include "Editor.h"
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
	
	this->AddInput("Exec", {});
	this->AddOutput("Out", {});
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

void UGameFlowNode::PostCDOContruct()
{
	UObject::PostCDOContruct();
	OnNodeCompiled.Broadcast();
}

void UGameFlowNode::PostCDOCompiled(const FPostCDOCompiledContext& Context)
{
	UObject::PostCDOCompiled(Context);
	OnNodeCompiled.Broadcast();
}

void UGameFlowNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	const bool bIsArray = PropertyChangedEvent.Property->ArrayDim == 1;
	const FName& PropertyName = PropertyChangedEvent.GetPropertyName();
	const int32 ArrayIndex = PropertyChangedEvent.GetArrayIndex(PropertyName.ToString());
	
	switch(PropertyChangedEvent.ChangeType)
	{
	default: break;

	case EPropertyChangeType::ValueSet:
		{
			// Is the modified property inside a TArray?
			if(bIsArray)
			{
				const FName OldPropertyValue = Temp_OldPinArray[ArrayIndex];
				FName NewPropertyValue;
				
				if(PropertyName.IsEqual("InputPins"))
				{
					const FGameFlowPinNodePair Connection = Inputs.FindRef(OldPropertyValue);
					NewPropertyValue = InputPins[ArrayIndex];
					const bool bIsChanged = !NewPropertyValue.IsEqual(OldPropertyValue);
					if(bIsChanged)
					{
						Inputs.Remove(OldPropertyValue);
						AddCompiledInput(NewPropertyValue, Connection);
					}
				}
				else if(PropertyName.IsEqual("OutputPins"))
				{
					const FGameFlowPinNodePair Connection = Outputs.FindRef(OldPropertyValue);
					NewPropertyValue = OutputPins[ArrayIndex];
					const bool bIsChanged = !NewPropertyValue.IsEqual(OldPropertyValue);
					if(bIsChanged)
					{
						Outputs.Remove(OldPropertyValue);
						AddCompiledOutput(NewPropertyValue, Connection);
					}
				}
		    }
	    }
    
	case EPropertyChangeType::ArrayRemove:
		{
			if(bIsArray)
			{
				const FName OldPropertyValue = Temp_OldPinArray[ArrayIndex];
				if(PropertyName.IsEqual("InputPins"))
				{
					RemoveInputPin(OldPropertyValue);
				}
				else if(PropertyName.IsEqual("OutputPins"))
				{
					RemoveOutput(OldPropertyValue);
				}
			}
		}
	}
}

void UGameFlowNode::PreEditChange(FProperty* PropertyAboutToChange)
{
	UObject::PreEditChange(PropertyAboutToChange);
	
	if(PropertyAboutToChange->ArrayDim == 1)
	{
		Temp_OldPinArray = *PropertyAboutToChange->ContainerPtrToValuePtr<TArray<FName>>(this);
	}
}

void UGameFlowNode::ValidateAsset()
{
	TArray<FName> InputKeys;
	Inputs.GetKeys(InputKeys);
	for(const FName& InputPinKey : InputKeys)
	{
		// If there's a mismatch between the input keys
		// and input pins array, correct it.
		if(!InputPins.Contains(InputPinKey))
		{
			Inputs.Remove(InputPinKey);
		}
	}

	TArray<FName> OutputKeys;
	Outputs.GenerateKeyArray(OutputKeys);
	for(const FName& OutputPinKey : OutputKeys)
	{
		// If there's a mismatch between the output keys
		// and output pins array, correct it.
		if(!OutputPins.Contains(OutputPinKey))
		{
			Outputs.Remove(OutputPinKey);
		}
	}
}

void UGameFlowNode::AddInput(const FName PinName, const FGameFlowPinNodePair Input)
{
	// Add input pin if not already present.
	InputPins.AddUnique(PinName);
	AddCompiledInput(PinName, Input);
}

void UGameFlowNode::AddCompiledInput(const FName PinName, const FGameFlowPinNodePair Input)
{
	const bool bValidOutput = Input.Node != nullptr && !Input.InputPinName.IsNone();
	const bool bRecursiveOutput = Input.Node == this;
	// Map pins only if the input is valid and non-recursive, otherwise ignore mapping.
	if(bValidOutput && !bRecursiveOutput)
	{
		Inputs.Add(PinName, Input);
		
		UGameFlowNode* InputNode = Input.Node;
		InputNode->Outputs.Add(Input.InputPinName, FGameFlowPinNodePair(PinName, this));
	}
}

void UGameFlowNode::RemoveInputPin(const FName PinName)
{
	const FGameFlowPinNodePair Pair = Inputs.FindRef(PinName);
	UGameFlowNode* ConnectedNode = Pair.Node;
	// Remove from other connected node.
	if(ConnectedNode != nullptr)
	{
		ConnectedNode->Outputs.Remove(Pair.InputPinName);
	}

	// Remove from this node.
	InputPins.Remove(PinName);
	Inputs.Remove(PinName);
}

void UGameFlowNode::AddOutput(const FName PinName, const FGameFlowPinNodePair Output)
{
	// Add output pin if not already present.
	OutputPins.AddUnique(PinName);
	AddCompiledOutput(PinName, Output);
}

void UGameFlowNode::AddCompiledOutput(const FName PinName, const FGameFlowPinNodePair Output)
{
	const bool bValidOutput = Output.Node != nullptr && !Output.InputPinName.IsNone();
	const bool bRecursiveOutput = Output.Node == this;
	// Map pins only if the output is valid and non-recursive, otherwise ignore mapping.
	if(bValidOutput && !bRecursiveOutput)
	{
		Outputs.Add(PinName, Output);

		UGameFlowNode* InputNode = Output.Node;
		InputNode->Inputs.Add(Output.InputPinName, FGameFlowPinNodePair(PinName, this));
	}
}

void UGameFlowNode::RemoveOutput(const FName PinName)
{
	const FGameFlowPinNodePair Pair = Outputs.FindRef(PinName);
	UGameFlowNode* ConnectedNode = Pair.Node;
	// Remove from other connected node.
	if(ConnectedNode != nullptr)
	{
		ConnectedNode->Inputs.Remove(Pair.InputPinName);
	}
	
	OutputPins.Remove(PinName);
	Outputs.Remove(PinName);
}

#endif
