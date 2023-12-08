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
	for(FName Pin : PinNames)
	{
		FString PinNameText = Pin.ToString();
		if(PinNameText.Contains("Exec") || PinNameText.Contains("Out")) Pin = EName::None;
		// Create logical pin and add it to the node pins list.
		CreatePin(PinDirection, PinCategory, Pin);
	}
}

UEdGraphPin* UGameFlowGraphNode::CreateNodePin(const EEdGraphPinDirection PinDirection, FName PinName)
{
	const FEdGraphPinType PinType = GetGraphPinType();
	const FString PinPrefix = PinDirection == EGPD_Input? "Exec_" : "Out_";
	
	FString PinNameText = PinName.ToString();
	if(PinNameText.Contains("Exec") || PinNameText.Contains("Out")) PinName = EName::None;
	
	PinName = FName(FString::Printf(TEXT("%s %d"), *PinPrefix, Pins.Num()));
	UEdGraphPin* Pin = CreatePin(PinDirection, PinType, PinName);

	return Pin;
}


