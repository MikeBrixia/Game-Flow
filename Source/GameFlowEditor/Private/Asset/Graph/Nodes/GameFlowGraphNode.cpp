// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "GameFlowEditor.h"
#include "GameFlowAsset.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Config/FGameFlowNodeInfo.h"
#include "Config/GameFlowEditorSettings.h"
#include "Widget/Nodes/SGameFlowNode.h"

UGameFlowGraphNode::UGameFlowGraphNode()
{
}

void UGameFlowGraphNode::InitNode()
{
	// Vital assertions.
	checkf(NodeAsset != nullptr, TEXT("Node asset is invalid(nullptr)"));

	UGameFlowEditorSettings* Settings = UGameFlowEditorSettings::Get();
	Info = Settings->NodesTypes.FindChecked(NodeAsset->TypeName);
	
	NodeAsset->OnEditAsset.AddUObject(this, &UGameFlowGraphNode::OnAssetEdited);
}

void UGameFlowGraphNode::AllocateDefaultPins()
{
	Pins.Empty();
	
	// Create pins for graph node.
	CreateNodePins(EGPD_Input, NodeAsset->GetInputPins());
	CreateNodePins(EGPD_Output, NodeAsset->GetOutputPins());
}

FName UGameFlowGraphNode::CreateUniquePinName(FName SourcePinName) const
{
	FString GeneratedName = SourcePinName.ToString();
	int Number;
	if(GeneratedName.IsNumeric())
	{
		Number = FCString::Atoi(*GeneratedName);
		GeneratedName = FString::FromInt(Number + 1);
	}
	else
	{
		Number = GetNum(GeneratedName);
		GeneratedName = FString::Printf(TEXT("NewPin_%d"), Number + 1);
	}

	return FName(GeneratedName);
}

TSharedPtr<SGraphNode> UGameFlowGraphNode::CreateVisualWidget()
{
	// Use UCLASS display name attribute value as node title.
	const FText TitleText = GetNodeTitle(ENodeTitleType::EditableTitle);
	
	// Create and initialize node widget.
	return SNew(SGameFlowNode)
	       .Node(this)
	       .TitleText(TitleText);
}

FText UGameFlowGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NodeAsset->GetClass()->GetDisplayNameText();
}

bool UGameFlowGraphNode::CanUserDeleteNode() const
{
	const FText NodeDisplayName = NodeAsset->GetClass()->GetDisplayNameText();
	// User will be able to delete all types of nodes except 'Start' and 'Finish'
	return !(NodeDisplayName.EqualTo(INVTEXT("Start")) || NodeDisplayName.EqualTo(INVTEXT("Finish")));
}

void UGameFlowGraphNode::ReconstructNode()
{
	const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());

	const UGameFlowEditorSettings* GameFlowEditorSettings = UGameFlowEditorSettings::Get();
	Info = GameFlowEditorSettings->NodesTypes.FindRef(NodeAsset->TypeName);
	
	// Reallocate all node pins.
	AllocateDefaultPins();
	
	UGameFlowGraph& GameFlowGraph = *CastChecked<UGameFlowGraph>(GetGraph());
	// Recreate node connections.
	GraphSchema->RecreateNodeConnections(GameFlowGraph, this, TArray { EGPD_Input, EGPD_Output });
}

void UGameFlowGraphNode::OnAssetEdited()
{
	ReconstructNode();
	OnNodeAssetChanged.Broadcast();
}

void UGameFlowGraphNode::SetNodeAsset(UGameFlowNode* Node)
{
	NodeAsset = Node;
	// Read new info data from config using the new node asset type.
	UGameFlowEditorSettings* Settings = UGameFlowEditorSettings::Get();
	Info = Settings->NodesTypes.FindChecked(NodeAsset->TypeName);
	
	// Notify listeners that the node asset has been changed.
	if(OnNodeAssetChanged.IsBound())
	{
		OnNodeAssetChanged.Broadcast();
	}
}

void UGameFlowGraphNode::CreateNodePins(const EEdGraphPinDirection PinDirection, const TArray<FName> PinNames)
{
	// Create all input pins.
	for(const FName& PinName : PinNames)
	{
		// Create logical pin and add it to the node pins list.
		CreateNodePin(PinDirection, PinName);
	}
}

UEdGraphPin* UGameFlowGraphNode::CreateNodePin(const EEdGraphPinDirection PinDirection, FName PinName)
{
	const bool bIsPinNameInvalid = PinName.IsEqual(EName::None);
	// Update Node asset depending on the new pin direction.
	switch(PinDirection)
	{
		// Direction is not valid, do nothing.
	default: break;
	     // Add input pin to node asset.
	case EGPD_Input:
		{
			// Generated unique pin name following previous pin pattern.
			if(bIsPinNameInvalid)
			{
				TArray<FName> InputPins = NodeAsset->GetInputPins();
				const FName PreviousName = InputPins.Num() > 0 ? InputPins.Last() : "None";
				PinName = CreateUniquePinName(PreviousName);
			}
			NodeAsset->AddInput(PinName, {});
			break;
		}
		// Add output pin to node asset.	
	case EGPD_Output:
		{
			// Generated unique pin name following previous pin pattern.
			if(bIsPinNameInvalid)
			{
				TArray<FName> OutputPins = NodeAsset->GetOutputPins();
				const FName PreviousName = OutputPins.Num() > 0 ? OutputPins.Last() : "None";
				PinName = CreateUniquePinName(PreviousName);
			}
			NodeAsset->AddOutput(PinName, {});
			break;
		}
	}
	
	const FEdGraphPinType PinType = GetGraphPinType();
	// Create the pin object.
	UEdGraphPin* Pin = CreatePin(PinDirection, PinType, PinName);
	Pin->PinFriendlyName = FText::FromName(PinName);
	return Pin;
}

