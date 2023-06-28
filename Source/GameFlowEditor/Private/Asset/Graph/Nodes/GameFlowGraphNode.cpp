// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/Nodes/GameFlowGraphNode.h"

#include "GameFlowEditor.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Widget/Nodes/SFlowNode.h"

UGameFlowGraphNode::UGameFlowGraphNode()
{
}

void UGameFlowGraphNode::AllocateDefaultPins()
{
	UE_LOG(LogGameFlow, Display, TEXT("Allocating default pins..."));
	// Initialize output pin info.
	FEdGraphPinType OutputPinInfo = {};
	OutputPinInfo.PinCategory = UEdGraphSchema_K2::PC_Exec;
	// Create logical pin and add it to the node pins list.
	CreatePin(EGPD_Output, OutputPinInfo, FName(""));
}

TSharedPtr<SGraphNode> UGameFlowGraphNode::CreateVisualWidget()
{
	return SNew(SFlowNode)
	       .Node(this);
}

void UGameFlowGraphNode::InitNode()
{
	// Once everything has been initialized, create node widget.
	CreateVisualWidget();
}


