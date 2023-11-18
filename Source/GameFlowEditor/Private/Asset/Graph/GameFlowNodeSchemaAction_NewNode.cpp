// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowNodeSchemaAction_NewNode.h"
#include "Utils/UGameFlowNodeFactory.h"

UEdGraphNode* FGameFlowNodeSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                               const FVector2D Location, bool bSelectNewNode)
{
	UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(ParentGraph);
	
	// Create a brand new game flow graph node of supplied type.
	UGameFlowGraphNode* GraphNode = UGameFlowNodeFactory::CreateGraphNode(NodeClass, GameFlowGraph->GameFlowAsset, GameFlowGraph);
    GraphNode->NodePosX = Location.X;
	GraphNode->NodePosY = Location.Y;
	
	return GraphNode;
}

UEdGraphNode* FGameFlowNodeSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins,
	const FVector2D Location, bool bSelectNewNode)
{
	return FEdGraphSchemaAction::PerformAction(ParentGraph, FromPins, Location, bSelectNewNode);
}

