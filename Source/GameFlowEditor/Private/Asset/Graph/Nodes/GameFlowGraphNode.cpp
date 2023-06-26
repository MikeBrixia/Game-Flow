// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Widget/Nodes/SFlowNode.h"

TSharedPtr<SGraphNode> UGameFlowGraphNode::CreateVisualWidget()
{
	return SNew(SFlowNode)
	       .Node(this);
}


