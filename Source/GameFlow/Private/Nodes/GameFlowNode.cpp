// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/GameFlowNode.h"
#include "DiffResults.h"
#include "GameFlowAsset.h"
#include "Config/GameFlowSettings.h"
#include "Engine/StreamableManager.h"
#include "Nodes/Pins/OutPinHandles.h"

UGameFlowNode::UGameFlowNode()
{
#if WITH_EDITOR
	TypeName = "Default";
	bIsActive = false;
	bForceDebugView = false;
	bBreakpointPlaced = false;
	bCanAddInputPin = false;
	bCanAddOutputPin = false;
	bIsCommentBubbleActive = false;
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

FDiffResults UGameFlowNode::PinsDiff(const UGameFlowNode* OtherNode, TArray<FDiffSingleResult>& Diffs,
	EEdGraphPinDirection Direction) const
{
	FDiffResults Results (&Diffs);

	if (OtherNode == nullptr) return Results;
	
	if (Direction == EGPD_Input)
	{
		// Find all pins that the other node does not possess.
		for (const FName PinName : GetInputPinsNames())
		{
			if (!OtherNode->GetInputPinsNames().Contains(PinName))
			{
				FDiffSingleResult PinDiff;
				PinDiff.Diff = EDiffType::OBJECT_REQUEST_DIFF;
				PinDiff.Category = EDiffType::ADDITION;
				PinDiff.Object1 = const_cast<UGameFlowNode*>(this);
				PinDiff.Object2 = const_cast<UGameFlowNode*>(OtherNode);
				PinDiff.DisplayString = FText::FromString(PinName.ToString());
				Results.Add(PinDiff);
			}
		}

		// Find all pins that this node does not possess.
		for (const FName PinName : OtherNode->GetInputPinsNames())
		{
			if (!GetInputPinsNames().Contains(PinName))
			{
				FDiffSingleResult PinDiff;
				PinDiff.Diff = EDiffType::OBJECT_REQUEST_DIFF;
				PinDiff.Category = EDiffType::SUBTRACTION;
				PinDiff.Object1 = const_cast<UGameFlowNode*>(this);
				PinDiff.Object2 = const_cast<UGameFlowNode*>(OtherNode);
				PinDiff.DisplayString = FText::FromString(PinName.ToString());
				Results.Add(PinDiff);
			}
		}	
	}
    else if (Direction == EGPD_Output)
    {
    	for (const FName PinName : GetOutputPinsNames())
    	{
    		if (!OtherNode->GetOutputPinsNames().Contains(PinName))
    		{
    			FDiffSingleResult PinDiff;
    			PinDiff.Diff = EDiffType::OBJECT_REQUEST_DIFF;
    			PinDiff.Category = EDiffType::ADDITION;
    			PinDiff.Object1 = const_cast<UGameFlowNode*>(this);
    			PinDiff.Object2 = const_cast<UGameFlowNode*>(OtherNode);
    			PinDiff.DisplayString = FText::FromString(PinName.ToString());
    			Results.Add(PinDiff);
    		}
    	}

    	for (const FName PinName : OtherNode->GetOutputPinsNames())
    	{
    		if (!GetOutputPinsNames().Contains(PinName))
    		{
    			FDiffSingleResult PinDiff;
    			PinDiff.Diff = EDiffType::OBJECT_REQUEST_DIFF;
    			PinDiff.Category = EDiffType::SUBTRACTION;
    			PinDiff.Object1 = const_cast<UGameFlowNode*>(this);
    			PinDiff.Object2 = const_cast<UGameFlowNode*>(OtherNode);
    			PinDiff.DisplayString = FText::FromString(PinName.ToString());
    			Results.Add(PinDiff);
    		}
    	} 
    }
	return Results;
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

void UGameFlowNode::AddPin(FName PinName, EEdGraphPinDirection PinDirection,
	TSubclassOf<UPinHandle> PinType, bool bCalledInsideConstructor)
{
	// If the pin obj type was not specified, read it from the node CDO.
	if (PinType == nullptr)
	{
		UGameFlowNode* Defaults = GetClass()->GetDefaultObject<UGameFlowNode>();
		PinType = Defaults->GetPinByName(PinName, PinDirection)->GetClass();
	}
	const FName PinObjectName = MakeUniqueObjectName(this, PinType, PinName);
	
	if (PinDirection == EGPD_Input)
	{
		UInputPinHandle* NewInputPin;
		if (bCalledInsideConstructor)
		{
			NewInputPin = CreateDefaultSubobject<UInputPinHandle>(PinName, false);
		}
		else
		{
			NewInputPin = NewObject<UInputPinHandle>(this, PinType, PinObjectName, RF_Transactional);
		}
		NewInputPin->PinName = PinName;
		Inputs.Add(PinName, NewInputPin);
	}
	else if (PinDirection == EGPD_Output)
	{
		UOutPinHandle* NewOutPin;
		if (bCalledInsideConstructor)
		{
			NewOutPin = CreateDefaultSubobject<UOutPinHandle>(PinName, false);
		}
		else
		{
			NewOutPin = NewObject<UOutPinHandle>(this, PinType, PinObjectName, RF_Transactional);
		}
		NewOutPin->PinName = PinName;
		Outputs.Add(PinName, NewOutPin);
	}
}

void UGameFlowNode::RemovePin(FName PinName, EEdGraphPinDirection PinDirection)
{
	if (PinDirection == EGPD_Input)
	{
		UPinHandle* PinHandle = GetPinByName(PinName, PinDirection);
		if (PinHandle != nullptr)
		{
			PinHandle->CutAllConnections();
			Inputs.Remove(PinName);
		}
	}
	else if (PinDirection == EGPD_Output)
	{
		UPinHandle* PinHandle = GetPinByName(PinName, PinDirection);
		if (PinHandle != nullptr)
		{
			PinHandle->CutAllConnections();
			Outputs.Remove(PinName);
		}
	}
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

void UGameFlowNode::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeChainProperty(PropertyChangedEvent);
	
	switch (PropertyChangedEvent.ChangeType)
	{
	default: break;
	
	case EPropertyChangeType::ValueSet:
		{
			FProperty* ChangedProperty = PropertyChangedEvent.Property;
			if (ChangedProperty == nullptr) return;
    		
			for (FProperty* ParentProp : PropertyChangedEvent.PropertyChain)
			{
				if (FMapProperty* MapProp = CastField<FMapProperty>(ParentProp))
				{
					void* MapContainer = MapProp->ContainerPtrToValuePtr<void>(this);
					
					FScriptMapHelper MapHelper(MapProp, MapContainer);

					// Iterate entries to find the one that contains the ChangedProperty
					for (int32 i = 0; i < MapHelper.GetMaxIndex(); ++i)
					{
						if (!MapHelper.IsValidIndex(i)) continue;
						
						void* KeyPtr = MapHelper.GetKeyPtr(i);
						void* ValuePtr = MapHelper.GetValuePtr(i);
						
						if (FNameProperty* PinNameProperty = CastField<FNameProperty>(MapProp->KeyProp))
						{
							const FName PinName = PinNameProperty->GetPropertyValue(KeyPtr);
							
							FObjectProperty* ObjProperty = CastField<FObjectProperty>(MapProp->ValueProp);
							if (FNameProperty* NameProperty = CastField<FNameProperty>(
								ObjProperty->PropertyClass->FindPropertyByName("PinName")))
							{
								UObject* ObjValue = ObjProperty->GetObjectPropertyValue(ValuePtr);
								TCHAR* NewName = FString::Printf(TEXT("%s"),
									*PinName.ToString()).GetCharArray().GetData();

								if (ObjValue != nullptr)
								{
									void* MemberPtr = NameProperty->ContainerPtrToValuePtr<void>(ObjValue);
									NameProperty->SetPropertyValue(MemberPtr, PinName);
								}
							}
						}
					}
				}
			}
			break;
		}
		
	case EPropertyChangeType::ArrayAdd:
		{
			auto PinsContainerProperty = PropertyChangedEvent.PropertyChain.GetActiveMemberNode();
			if (FMapProperty* MapProperty = CastField<FMapProperty>(PinsContainerProperty->GetValue()))
			{
				FScriptMapHelper_InContainer MapHelper(MapProperty, this);

				int LastIndex = MapHelper.GetMaxIndex() - 1;
				uint8* PinNamePtr = MapHelper.GetKeyPtr(LastIndex);

				if (FNameProperty* PinNameProperty = CastField<FNameProperty>(MapProperty->KeyProp))
				{
					// Choose what type of pin to create.
					if (MapProperty->GetName().Equals("Inputs"))
					{
						PinNameProperty->SetPropertyValue(PinNamePtr, *("NewInputPin" + FString::FromInt(LastIndex)));
					}
					else if (MapProperty->GetName().Equals("Outputs"))
					{
						PinNameProperty->SetPropertyValue(PinNamePtr, *("NewOutputPin" + FString::FromInt(LastIndex)));
					}
				}
			}
			break;
		}

		case EPropertyChangeType::ArrayRemove:
		{
			auto PinsContainerProperty = PropertyChangedEvent.PropertyChain.GetActiveMemberNode();
			if (FMapProperty* MapProperty = CastField<FMapProperty>(PinsContainerProperty->GetValue()))
			{
				FScriptMapHelper_InContainer MapHelper(MapProperty, this);
				// TODO: Break pin handle connections on pin removal.
			}
		}
	}
}

void UGameFlowNode::AddInputPin_CDO(FName PinName)
{
	UInputPinHandle* NewInputPin = CreateDefaultSubobject<UInputPinHandle>(PinName, false);
	NewInputPin->PinName = PinName;
	Inputs.Add(PinName, NewInputPin);
}

void UGameFlowNode::AddOutputPin_CDO(FName PinName)
{
	UOutPinHandle* NewPin = CreateDefaultSubobject<UOutPinHandle>(PinName, false);
	NewPin->PinName = PinName;
	Outputs.Add(PinName, NewPin);
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
	if(bBreakpointPlaced)
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