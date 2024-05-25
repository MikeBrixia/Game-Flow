// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/GameFlowNode.h"
#include "GameFlowAsset.h"
#include "Config/GameFlowSettings.h"

UGameFlowNode::UGameFlowNode()
{
	TypeName = "Event";
}

void UGameFlowNode::FinishExecute(bool bFinish)
{
	UGameFlowAsset* OwnerAsset = GetTypedOuter<UGameFlowAsset>();
	
	// If node has finished executing, remove it from asset active nodes.
	if(bFinish && OwnerAsset != nullptr)
	{
		OnFinishExecute();
		OwnerAsset->RemoveActiveNode(this);
	}
}

void UGameFlowNode::ExecuteOutputPin(FName PinName)
{
	const FPinHandle OutputPinHandle = Outputs.FindRef(PinName);
	UGameFlowAsset* OwnerAsset = GetTypedOuter<UGameFlowAsset>();

	for(const auto& Pair: OutputPinHandle.Connections)
	{
		const FPinConnectionInfo& ConnectionInfo = Pair.Value;
		UGameFlowNode* NextNode = ConnectionInfo.DestinationObject;
		// If valid, activate next node and start executing it.
		if(NextNode != nullptr)
		{
			// Execute the next node.
			OwnerAsset->AddActiveNode(NextNode);

			FName PinToExecute = ConnectionInfo.DestinationPinName;
			NextNode->Execute(PinToExecute);
		}
	}
}

#if WITH_EDITOR

TArray<FName> UGameFlowNode::GetInputPinsNames() const
{
	TArray<FName> InputPins;
	Inputs.GenerateKeyArray(InputPins);
	return InputPins;
}

TArray<FName> UGameFlowNode::GetOutputPinsNames() const
{
	TArray<FName> OutputPins;
	Outputs.GenerateKeyArray(OutputPins);
	return OutputPins;
}

TArray<FName> UGameFlowNode::GetNodeTypeOptions() const
{
	return UGameFlowSettings::Get()->Options; 
}

bool UGameFlowNode::CanAddInputPin() const
{
	return bCanAddInputPin;
}

bool UGameFlowNode::CanAddOutputPin() const
{
	return bCanAddOutputPin; 
}

void UGameFlowNode::AddInputPin(FName PinName)
{
	if(!PinName.IsNone() && PinName.IsValid())
	{
		Inputs.Add(PinName, {PinName, this, EGPD_Input});
	}
}

void UGameFlowNode::RemoveInputPin(FName PinName)
{
	Inputs.Remove(PinName);
}

void UGameFlowNode::AddOutputPin(FName PinName)
{
	if(!PinName.IsNone() && PinName.IsValid())
	{
		Outputs.Add(PinName, {PinName, this, EGPD_Output});
	}
}

void UGameFlowNode::RemoveOutputPin(FName PinName)
{
	Outputs.Remove(PinName);
}

void UGameFlowNode::UpdatePinHandle(const FPinHandle& UpdatedPinHandle)
{
	switch(UpdatedPinHandle.PinDirection)
	{
	default: break;

	case EGPD_Input:
		Inputs[UpdatedPinHandle.PinName] = UpdatedPinHandle;
		break;

	case EGPD_Output:
		Outputs[UpdatedPinHandle.PinName] = UpdatedPinHandle;
		break;
	}
}

FPinHandle UGameFlowNode::GetPinByName(FName PinName, TEnumAsByte<EEdGraphPinDirection> Direction) const
{
	FPinHandle PinHandle;
	
	switch(Direction)
	{
	default: break;

	case EGPD_Input:
		PinHandle = Inputs.FindRef(PinName);
		break;

	case EGPD_Output:
		PinHandle = Outputs.FindRef(PinName);
		break;
	}

	return PinHandle;
}

void UGameFlowNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	// Check if modified property is valid.
	// FProperty could be null when replacing game flow node references,
	// in particular if pins connections are different between deleted and replacement nodes.
	if(PropertyChangedEvent.Property != nullptr)
	{
		FName PinName = PropertyChangedEvent.GetPropertyName();
		
		TMap<FName, FPinHandle> Pins;
		if(PinName.IsEqual("Inputs"))
		{
			Pins = Inputs;
		}
		else if(PinName.IsEqual("Outputs"))
		{
			Pins = Outputs;
		}
		
		switch(PropertyChangedEvent.ChangeType)
		{
		default: break;

		case EPropertyChangeType::ArrayAdd:
			{
				for(const auto& Pair : Pins)
				{
					FPinHandle PinHandle = Pair.Value;
					PinHandle.PinName = Pair.Key;
					PinHandle.PinOwner = this;
					UpdatePinHandle(PinHandle);
				}
				break;
			}
			
		case EPropertyChangeType::ValueSet:
			{
				for(const auto& Pair : Pins)
				{
					FPinHandle PinHandle = Pair.Value;
					PinHandle.PinName = Pair.Key;
					PinHandle.PinOwner = this;
					UpdatePinHandle(PinHandle);
				}
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

#endif
