// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Config/FGameFlowNodeInfo.h"
#include "Config/GameFlowEditorSettings.h"
#include "Widget/Nodes/SGameFlowNode.h"

UGameFlowGraphNode::UGameFlowGraphNode()
{
}

void UGameFlowGraphNode::AllocateDefaultPins()
{
	const FEdGraphPinType OutputPinInfo = GetGraphPinType();
	
	// Create pins for graph node.
	CreateNodePins(OutputPinInfo, EGPD_Input, NodeAsset->GetInputPins());
	CreateNodePins(OutputPinInfo, EGPD_Output, NodeAsset->GetOutputPins());
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

void UGameFlowGraphNode::InitNode()
{
	// Vital assertions.
	checkf(NodeAsset != nullptr, TEXT("Node asset is invalid(nullptr)"));

	UGameFlowEditorSettings* Settings = UGameFlowEditorSettings::Get();
	Info = Settings->NodesTypes.FindChecked(NodeAsset->TypeName);
}

void UGameFlowGraphNode::CreateNodePins(const FEdGraphPinType PinCategory, const EEdGraphPinDirection PinDirection,
	const TArray<FName> PinNames)
{
	// Create all input pins.
	for(const FName& Pin : PinNames)
	{
		// Create logical pin and add it to the node pins list.
		CreatePin(PinDirection, PinCategory, Pin);
	}
}

UEdGraphPin* UGameFlowGraphNode::CreateNodePin(const EEdGraphPinDirection PinDirection, FName PinName)
{
	const FEdGraphPinType PinType = GetGraphPinType();

	// When name is 'None', use a generated one.
	if(PinName.IsEqual(EName::None))
	{
		// Generated pin name structure will be following. '{Prefix}_{CurrentPinsNumber};
		// For example it could be: "Exec_2", for the second input pin.
		const FString PinPrefix = PinDirection == EGPD_Input? "Exec_" : "Out_";
		FString PinNameString = FString::Printf(TEXT("%s %d"), *PinPrefix, Pins.Num());
		PinName = FName(PinNameString);
	}
	UEdGraphPin* Pin = CreatePin(PinDirection, PinType, PinName);

	// Update Node asset depending on the new pin direction.
	switch(PinDirection)
	{
		// Direction is not valid, do nothing.
	default:
		break;
	     // Add input pin to node asset.
	case EGPD_Input:
		{
			TArray<FName> InputPins = NodeAsset->GetInputPins();
			InputPins.Add(PinName);
			break;
		}
		// Add input pin to node asset.	
	case EGPD_Output:
		{
			NodeAsset->AddOutput(PinName, nullptr);
			break;
		}
	}
	
	return Pin;
}


