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

	for(const auto& ConnectionInfo: OutputPinHandle.Connections)
	{
		UGameFlowNode* NextNode = ConnectionInfo.Node;
		// If valid, activate next node and start executing it.
		if(NextNode != nullptr)
		{
			// Execute the next node.
			OwnerAsset->AddActiveNode(NextNode);

			FName PinToExecute = ConnectionInfo.OtherPinName;
			NextNode->Execute(PinToExecute);
		}
	}
}

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

#if WITH_EDITOR

void UGameFlowNode::AddInputPin(FName PinName)
{
	if(!PinName.IsNone() && PinName.IsValid())
	{
		Inputs.Add(PinName, {PinName});
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
		Outputs.Add(PinName, {PinName});
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
		switch(PropertyChangedEvent.ChangeType)
		{
		default: break;
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
