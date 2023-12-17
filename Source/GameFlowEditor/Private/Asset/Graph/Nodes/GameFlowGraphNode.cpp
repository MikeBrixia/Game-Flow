// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "GameFlowAsset.h"
#include "Config/FGameFlowNodeInfo.h"
#include "Config/GameFlowEditorSettings.h"
#include "Widget/Nodes/SGameFlowNode.h"

UGameFlowGraphNode::UGameFlowGraphNode()
{
}

void UGameFlowGraphNode::AllocateDefaultPins()
{
	// Create pins for graph node.
	CreateNodePins(EGPD_Input, NodeAsset->GetInputPins());
	CreateNodePins(EGPD_Output, NodeAsset->GetOutputPins());
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
	
	// Each time a node pin name or type gets modified, reconstruct the node to keep it updated.
	//NodeAsset->OnNodePinNameChange.AddUObject(this, &UGameFlowGraphNode::ReconstructNode);
	NodeAsset->OnNodeTypeChange.AddLambda([=](const FName& NewTypeName)
	{
		// Update node info from settings using the new typename, then reconstruct the node.
	    Info = Settings->NodesTypes.FindChecked(NewTypeName);
		ReconstructNode();
	});
}

bool UGameFlowGraphNode::CanUserDeleteNode() const
{
	const FText NodeDisplayName = NodeAsset->GetClass()->GetDisplayNameText();
	// User will be able to delete all types of nodes except 'Start' and 'Finish'
	return !(NodeDisplayName.EqualTo(INVTEXT("Start")) || NodeDisplayName.EqualTo(INVTEXT("Finish")));
}

void UGameFlowGraphNode::ReconstructNode()
{
	Super::ReconstructNode();
	
	// Reallocate all node pins.
	Pins.Empty();
	AllocateDefaultPins();
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
	// When name is 'None', use a generated one.
	if(PinName.IsEqual(EName::None))
	{
		PinName = NodeAsset->GenerateAddPinName(PinDirection);
	}
	const FEdGraphPinType PinType = GetGraphPinType();
	UEdGraphPin* Pin = CreatePin(PinDirection, PinType, PinName);
	Pin->PinFriendlyName = FText::FromName(PinName);
	
	// Update Node asset depending on the new pin direction.
	switch(PinDirection)
	{
		// Direction is not valid, do nothing.
	default: break;
	     // Add input pin to node asset.
	case EGPD_Input:
		{
			NodeAsset->AddInput(PinName);
			break;
		}
		// Add output pin to node asset.	
	case EGPD_Output:
		{
			NodeAsset->AddOutput(PinName, {});
			break;
		}
	}
	return Pin;
}

