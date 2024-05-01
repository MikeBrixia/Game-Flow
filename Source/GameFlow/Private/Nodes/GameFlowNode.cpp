﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/GameFlowNode.h"
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
	// By default, directly execute default output pin
	FinishExecute("Out", true);
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

	if(NextNode != nullptr)
	{
		// Execute the next node.
		OwnerAsset->AddActiveNode(NextNode);
		NextNode->Execute(ConnectionPinName);
	}
}

#if WITH_EDITOR

void UGameFlowNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	// Check if modified property is valid.
	// FProperty could be null when replacing game flow node references,
	// in particular if pins connections are different between deleted and replacement nodes.
	if(PropertyChangedEvent.Property != nullptr)
	{
		const FName& PropertyName = PropertyChangedEvent.GetPropertyName();
		const int32 ArrayIndex = PropertyChangedEvent.GetArrayIndex(PropertyName.ToString());
	    
		switch(PropertyChangedEvent.ChangeType)
		{
		default: break;
		
		case EPropertyChangeType::ValueSet:
			{
				if (PropertyName.IsEqual("InputPins"))
				{
					const FName OldPropertyValue = Temp_OldPinArray[ArrayIndex];
					OnInputPinValueSet(OldPropertyValue, ArrayIndex);
				}
				else if (PropertyName.IsEqual("OutputPins"))
				{
					const FName OldPropertyValue = Temp_OldPinArray[ArrayIndex];
					OnOutputPinValueSet(OldPropertyValue, ArrayIndex);
				}
				break;
			}
    
		case EPropertyChangeType::ArrayRemove:
			{
				const FName OldPropertyValue = Temp_OldPinArray[ArrayIndex];
				OnPinRemoved(OldPropertyValue);
				break;
			}

			// Notify listeners this asset has been redirected.
		case EPropertyChangeType::Redirected:
			{
				OnAssetRedirected.Broadcast();
				break;
			}
		}
	}
}

void UGameFlowNode::PreEditChange(FProperty* PropertyAboutToChange)
{
	UObject::PreEditChange(PropertyAboutToChange);
	
	if(PropertyAboutToChange != nullptr)
	{
		const FName PropertyName = PropertyAboutToChange->GetFName();
		const bool bIsPinArray = PropertyAboutToChange->ArrayDim == 1 && (PropertyName.IsEqual("InputPins")
		                         || PropertyName.IsEqual("OutputPins"));
		
		const UClass* OwnerClass = PropertyAboutToChange->GetOwner<UClass>();
		const bool bIsValidOwnerClass = OwnerClass != nullptr
									    && PropertyAboutToChange->GetOwner<UClass>() != UBlueprintCore::StaticClass();
		
		if(bIsPinArray && bIsValidOwnerClass)
		{
			Temp_OldPinArray = *PropertyAboutToChange->ContainerPtrToValuePtr<TArray<FName>>(this);
		}
	}
}

void UGameFlowNode::OnInputPinValueSet(FName PinName, int PinArrayIndex)
{
	const FGameFlowPinNodePair Connection = Inputs.FindRef(PinName);
	FName NewPropertyValue = InputPins[PinArrayIndex];
	const bool bIsChanged = !NewPropertyValue.IsEqual(PinName);
	if(bIsChanged)
	{
		const FGameFlowPinNodePair InputPair = Inputs.FindRef(PinName);
		
		RemoveInputPort(PinName);
		AddInputPort(NewPropertyValue, Connection);

		UGameFlowNode* ConnectedNode = InputPair.Node;
		// Update connected node pins accordingly.
		if(ConnectedNode != nullptr)
		{
			const FName ConnectedPinName = InputPair.InputPinName;
			ConnectedNode->AddOutputPort(ConnectedPinName, {NewPropertyValue, this});
		}
	}
}

void UGameFlowNode::OnOutputPinValueSet(FName PinName, int PinArrayIndex)
{
	const FGameFlowPinNodePair Connection = Outputs.FindRef(PinName);
	FName NewPropertyValue = OutputPins[PinArrayIndex];
	const bool bIsChanged = !NewPropertyValue.IsEqual(PinName);
	if(bIsChanged)
	{
		const FGameFlowPinNodePair OutputPair = Inputs.FindRef(PinName);
		
		RemoveOutputPort(PinName);
		AddOutputPort(NewPropertyValue, Connection);

		UGameFlowNode* ConnectedNode = OutputPair.Node;
		// Update connected node pin accordingly.
		if(ConnectedNode != nullptr)
		{
			const FName ConnectedPinName = OutputPair.InputPinName;
			ConnectedNode->AddInputPort(ConnectedPinName, {NewPropertyValue, this});
		}
	}
}

void UGameFlowNode::OnPinRemoved(FName PinName)
{
	if(Inputs.Contains(PinName))
	{
		const FGameFlowPinNodePair PinNodePair = Inputs.FindRef(PinName);
		RemoveInputPin(PinName);

		UGameFlowNode* ConnectedNode = PinNodePair.Node;
		if(ConnectedNode != nullptr)
		{
			ConnectedNode->RemoveOutputPort(PinNodePair.InputPinName);
		}
	}
	else if(Outputs.Contains(PinName))
	{
		const FGameFlowPinNodePair PinNodePair = Outputs.FindRef(PinName);
		RemoveOutputPin(PinName);

		UGameFlowNode* ConnectedNode = PinNodePair.Node;
		if(ConnectedNode != nullptr)
		{
			ConnectedNode->RemoveInputPort(PinNodePair.InputPinName);
		}
	}
}

void UGameFlowNode::AddInputPin(const FName PinName, const FGameFlowPinNodePair Input)
{
	// Add input pin if not already present.
	InputPins.AddUnique(PinName);
	AddInputPort(PinName, Input);
}

void UGameFlowNode::AddInputPort(const FName PinName, const FGameFlowPinNodePair Input)
{
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
	// Remove from this node.
	InputPins.Remove(PinName);
	RemoveInputPort(PinName);
}

void UGameFlowNode::RemoveInputPort(FName PinName)
{
	Inputs.Remove(PinName);
}

void UGameFlowNode::AddOutputPin(const FName PinName, const FGameFlowPinNodePair Output)
{
	// Add output pin if not already present.
	OutputPins.AddUnique(PinName);
	AddOutputPort(PinName, Output);
}

void UGameFlowNode::AddOutputPort(const FName PinName, const FGameFlowPinNodePair Output)
{
	const bool bValidOutput = Output.Node != nullptr && !Output.InputPinName.IsNone();
	const bool bRecursiveOutput = Output.Node == this;
	// Map pins only if the output is valid and non-recursive, otherwise ignore mapping.
	if(bValidOutput && !bRecursiveOutput)
	{
		// Connect this node output port port to the other node input port.
		Outputs.Add(PinName, Output);
	}
}

void UGameFlowNode::RemoveOutputPin(const FName PinName)
{
	OutputPins.Remove(PinName);
	RemoveOutputPort(PinName);
}

void UGameFlowNode::RemoveOutputPort(FName PinName)
{
	Outputs.Remove(PinName);
}

#endif
