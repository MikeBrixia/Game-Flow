// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/GameFlowNode.h"
#include "GameFlowAsset.h"
#include "Config/GameFlowSettings.h"
#include "Engine/StreamableManager.h"
#include "GameFlowEditor/Public/GameFlowEditor.h"
#include "Nodes/Pins/OutPinHandles.h"

UGameFlowNode::UGameFlowNode()
{
#if WITH_EDITOR
	TypeName = "Default";
	bIsActive = false;
	bForceDebugView = false;
	bBreakpointEnabled = false;
	bCanAddInputPin = false;
	bCanAddOutputPin = false;
#endif
}

#if WITH_EDITOR

#include "AssetViewUtils.h"
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

TArray<UPinHandle*> UGameFlowNode::GetPinsByDirection(TEnumAsByte<EEdGraphPinDirection> Direction) const
{
	TArray<UPinHandle*> Pins;
	
	switch (Direction)
	{
	default: break;

	case EGPD_Input:
		{
			TArray<UInputPinHandle*> InputPins;
			Inputs.GenerateValueArray(InputPins);
			Pins.Append(InputPins);
			break;
		}

	case EGPD_Output:
		{
			TArray<UOutPinHandle*> OutputPins;
			Outputs.GenerateValueArray(OutputPins);
			Pins.Append(OutputPins);
			break;
		}
	}

	return Pins;
}

void UGameFlowNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	
	if(PropertyChangedEvent.ChangeType == EPropertyChangeType::Redirected)
	{
		if (OnAssetRedirected.IsBound())
		{
			OnAssetRedirected.Broadcast();
		}
	}
}

void UGameFlowNode::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeChainProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayAdd)
	{
		auto PinsContainerProperty = PropertyChangedEvent.PropertyChain.GetActiveMemberNode();
		if (FMapProperty* MapProperty = CastField<FMapProperty>(PinsContainerProperty->GetValue()))
		{
			FScriptMapHelper_InContainer MapHelper(MapProperty, this);

			int LastIndex = MapHelper.GetMaxIndex() - 1; 
			uint8* PinNamePtr = MapHelper.GetKeyPtr(LastIndex);
            uint8* PinHandlePtr = MapHelper.GetValuePtr(LastIndex);
			
			if (FNameProperty* PinNameProperty = CastField<FNameProperty>(MapProperty->KeyProp))
			{
				// Give the map key a default initialization value.
				PinNameProperty->SetPropertyValue(PinNamePtr, "NewPin");
				const FName PinName = PinNameProperty->GetPropertyValue(PinNamePtr);
				
				UPinHandle* PinHandle = nullptr;
				// Choose what type of pin to create.
				if(MapProperty->GetName().Equals("Inputs"))
				{
					PinHandle = CreateExecInputPin(PinName);
				}
				else if(MapProperty->GetName().Equals("Outputs"))
				{
					PinHandle = CreateExecOutputPin(PinName);
				}
				FObjectProperty* ObjProperty = CastField<FObjectProperty>(MapProperty->ValueProp);
				// Map value property will now point at the created pin handle.
				ObjProperty->SetObjectPropertyValue(PinHandlePtr, PinHandle);
			}
		}
	}
	else if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ValueSet)
	{
		
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

#else

TArray<FName> UGameFlowNode::GetNodeTypeOptions() const
{
	return {}; 
}

#endif

void UGameFlowNode::TriggerOutputPin(FName PinName)
{
	UOutPinHandle* OutputPinHandle = Outputs.FindRef(PinName);
	OutputPinHandle->TriggerPin();
}

void UGameFlowNode::FinishExecute(bool bFinish)
{
	UGameFlowAsset* OwnerAsset = GetTypedOuter<UGameFlowAsset>();
	
	// If node has finished executing, remove it from asset active nodes.
	if(bFinish && OwnerAsset != nullptr)
	{
#if WITH_EDITOR
		OwnerAsset->RemoveActiveNode(this);
#endif
		OnFinishExecute();
	}
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
		FStreamableManager StreamableManager;
		UGameFlowAsset* LoadedTemplate = StreamableManager.LoadSynchronous(OwnerAsset->TemplateAsset);
		AssetViewUtils::OpenEditorForAsset(LoadedTemplate);
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