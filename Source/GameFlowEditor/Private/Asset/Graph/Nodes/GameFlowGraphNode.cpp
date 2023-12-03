// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Config/FGameFlowNodeInfo.h"
#include "Config/GameFlowEditorSettings.h"
#include "Widget/Nodes/SFlowNode.h"

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
	UGameFlowEditorSettings* Settings = UGameFlowEditorSettings::Get();
	const FGameFlowNodeInfo NodeSettings = Settings->NodesTypes.FindChecked(NodeAsset->TypeName);
	
	const FText TitleText = NodeAsset->GetClass()->GetDisplayNameText();
	// Create and initialize node widget.
	return SNew(SGameFlowNode)
	       .Node(this)
	       .TitleBackgroundColor(NodeSettings.TitleBarColor)
	       .TitleText(TitleText);
}

FText UGameFlowGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return Super::GetNodeTitle(TitleType);
}

void UGameFlowGraphNode::InitNode()
{
	// Vital assertions.
	checkf(NodeAsset != nullptr, TEXT("Node asset is invalid(nullptr)"));
	
	// Once everything has been initialized, create node widget.
	CreateVisualWidget();
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


