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
	FEdGraphPinType OutputPinInfo = {};
	OutputPinInfo.PinCategory = UEdGraphSchema_K2::PC_Exec;
	
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
	for(FName Pin : PinNames)
	{
		if(Pin.IsEqual("Exec") || Pin.IsEqual("Out")) Pin = EName::None;
		// Create logical pin and add it to the node pins list.
		CreatePin(PinDirection, PinCategory, Pin);
	}
}


