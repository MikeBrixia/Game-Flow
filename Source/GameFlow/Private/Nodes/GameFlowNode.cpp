// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/GameFlowNode.h"
#include "AssetViewUtils.h"
#include "GameFlowAsset.h"
#include "Config/GameFlowSettings.h"
#include "Nodes/Pins/OutPinHandles.h"

UGameFlowNode::UGameFlowNode()
{
	TypeName = "Default";
	bIsActive = false;
	bForceDebugView = false;
}

void UGameFlowNode::TryExecute(FName PinName)
{
#if WITH_EDITOR
	UGameFlowAsset* OwnerAsset = GetTypedOuter<UGameFlowAsset>();
	// Mark this node as active.
	OwnerAsset->AddActiveNode(this);

	// If we've hit a breakpoint, try opening the asset editor if it is closed.
	if(bBreakpointEnabled)
	{
		AssetViewUtils::OpenEditorForAsset(OwnerAsset->TemplateAsset);
	}
	
    if(OwnerAsset->TemplateAsset != nullptr)
    {
    	const UGameFlowNode* TemplateNode = OwnerAsset->TemplateAsset->GetNodeByGUID(GUID);
    	if(TemplateNode != nullptr && TemplateNode->OnAssetExecuted.IsBound())
    	{
    		TemplateNode->OnAssetExecuted.Broadcast(Inputs.FindRef(PinName));
    	}
    }
#endif
	Execute(PinName);
}

void UGameFlowNode::FinishExecute(bool bFinish)
{
	UGameFlowAsset* OwnerAsset = GetTypedOuter<UGameFlowAsset>();
	
	// If node has finished executing, remove it from asset active nodes.
	if(bFinish && OwnerAsset != nullptr)
	{
		OwnerAsset->RemoveActiveNode(this);
		OnFinishExecute();
	}
}

void UGameFlowNode::TriggerOutputPin(FName PinName)
{
	UOutPinHandle* OutputPinHandle = Outputs.FindRef(PinName);
	OutputPinHandle->TriggerPin();
}

#if WITH_EDITOR

#include "EdGraph/EdGraphNode.h"

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
		UInputPinHandle* NewInputPinHandle = CreateExecInputPin(PinName);
		Inputs.Add(PinName, NewInputPinHandle);
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
		UOutPinHandle* NewOutputPinHandle = CreateExecOutputPin(PinName);
		Outputs.Add(PinName, NewOutputPinHandle);
	}
}

void UGameFlowNode::RemoveOutputPin(FName PinName)
{
	Outputs.Remove(PinName);
}

UPinHandle* UGameFlowNode::GetPinByName(FName PinName, TEnumAsByte<EEdGraphPinDirection> Direction) const
{
	UPinHandle* PinHandle = nullptr;

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
		TMap<FName, UPinHandle*> Pins;
		
		switch(PropertyChangedEvent.ChangeType)
		{
			
		default: break;

			// Handle details panel input/output pins addition.
		case EPropertyChangeType::ArrayAdd:
			{
				if(PinName.IsEqual("Inputs"))
				{
				}
				else if(PinName.IsEqual("Outputs"))
				{
				}
			}

			// Handle pins renames in details panel.
		case EPropertyChangeType::ValueSet:
			{
				for(const auto& Pair : Pins)
				{
					UPinHandle* PinHandle = Pair.Value;
					PinHandle->PinName = Pair.Key;
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

UOutPinHandle* UGameFlowNode::CreateExecOutputPin(FName PinName)
{
	const FString NodeName = GetName();
	const FName FullPinName = FName(NodeName + "." + PinName.ToString());
	
	UOutPinHandle* NewOutPin = NewObject<UOutPinHandle>(this, FullPinName);
	NewOutPin->PinName = PinName;
    
	return NewOutPin;
}

UInputPinHandle* UGameFlowNode::CreateExecInputPin(FName PinName)
{
	const FString NodeName = GetName();
	const FName FullPinName = FName(NodeName + "." + PinName.ToString());
	
	UInputPinHandle* NewInputPin = NewObject<UInputPinHandle>(this, FullPinName);
	NewInputPin->PinName = PinName;

	return NewInputPin;
}

FName UGameFlowNode::GeneratePinName(FName PinName) const
{
	const FString NodeName = GetName();
	const FName FullPinName = FName(NodeName + "." + PinName.ToString());
	return FullPinName;
}

FString UGameFlowNode::GetCustomDebugInfo() const
{
	return "";
}

#endif
