// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/GameFlowNode.h"
#include "GameFlow.h"
#include "GameFlowAsset.h"
#include "Config/GameFlowSettings.h"

UGameFlowNode::UGameFlowNode()
{
	TypeName = "Event";
	bIsActive = false;
}

void UGameFlowNode::FinishExecute(bool bFinish)
{
	UGameFlowAsset* OwnerAsset = GetTypedOuter<UGameFlowAsset>();
	
	// If node has finished executing, remove it from asset active nodes.
	if(bFinish && OwnerAsset != nullptr)
	{
		OnFinishExecute();
		OwnerAsset->RemoveActiveNode(this);
		
		// Execute default output pin.
		ExecuteOutputPin("Out");
	}
}

void UGameFlowNode::ExecuteOutputPin(FName PinName)
{
	FPinHandle OutputPinHandle = Outputs.FindRef(PinName);
	UGameFlowAsset* OwnerAsset = GetTypedOuter<UGameFlowAsset>();
	
	for(const auto& Pair: OutputPinHandle.Connections)
	{
		FPinConnectionInfo ConnectionInfo = Pair.Value;
		UGameFlowNode* NextNode = ConnectionInfo.DestinationObject;
		// If valid, activate next node and start executing it.
		if(NextNode != nullptr)
		{
			// Execute the next node.
			OwnerAsset->AddActiveNode(NextNode);
			
			// Mark connection as active. This is mainly needed by game flow drawing policy.
			ConnectionInfo.bIsActive = true;
			OutputPinHandle.Connections[Pair.Key] = ConnectionInfo;
			UpdatePinHandle(OutputPinHandle);
			
			const FName PinToExecute = ConnectionInfo.DestinationPinName;
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

void UGameFlowNode::GetNodeIconInfo(FString& Key, FLinearColor& Color) const
{
	Color = FLinearColor::White;
	Key = "GameFlow.Editor.Default.Nodes.Icons." + TypeName.ToString();
}

bool UGameFlowNode::CanAddInputPin() const
{
	return bCanAddInputPin;
}

bool UGameFlowNode::CanAddOutputPin() const
{
	return bCanAddOutputPin; 
}

bool UGameFlowNode::IsActiveNode() const
{
	const UGameFlowAsset* ParentAsset = GetTypedOuter<UGameFlowAsset>();
	return ParentAsset->GetActiveNodes().Contains(this);
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

	// Retrieve all non-None pins.
	if(!PinName.IsNone())
	{
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
	}
	
	return PinHandle;
}

void UGameFlowNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	// Check if modified property is valid.
	// FProperty could be null when replacing game flow node references,
	// in particular if pins connections differs between deleted and replacement nodes.
	if(PropertyChangedEvent.Property != nullptr)
	{
		const FName PinName = PropertyChangedEvent.GetPropertyName();
		TMap<FName, FPinHandle> Pins;
		TArray<FName> PinsNames;
		EEdGraphPinDirection PinDirection = EGPD_MAX;
		
		if(PinName.IsEqual("Inputs"))
		{
			PinsNames = GetInputPinsNames();
			Pins = Inputs;
			PinDirection = EGPD_Input;
		}
		else if(PinName.IsEqual("Outputs"))
		{
			PinsNames = GetOutputPinsNames();
			Pins = Outputs;
			PinDirection = EGPD_Output;
		}
		
		switch(PropertyChangedEvent.ChangeType)
		{
		default: break;

			// Handle details panel input/output pins addition.
		case EPropertyChangeType::ArrayAdd:
			{
				const int KeyIndex = PropertyChangedEvent.GetArrayIndex(PinName.ToString());
				const FName Name = PinsNames[KeyIndex];
				FPinHandle& Handle = Pins[Name];
				// Initialize pin handle properties
				Handle.PinName = Name;
				Handle.PinDirection = PinDirection;
				break;
			}

			// Handle pins renames in details panel.
		case EPropertyChangeType::ValueSet:
			{
				for(const auto& Pair : Pins)
				{
					FPinHandle PinHandle = Pair.Value;
					PinHandle.PinName = Pair.Key;
					UpdatePinHandle(PinHandle);
				}
				break;
			}
			
			// Notify listeners(usually the associated graph node) this asset has been compiled/redirected.
		case EPropertyChangeType::Redirected:
			{
				OnAssetRedirected.Broadcast();
				break;
			}
		}
	}
}

#endif
