// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowNodeSchemaAction_NewNode.h"
#include "Asset/Graph/GameFlowGraphSchema.h"

UEdGraphNode* FGameFlowNodeSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                               const FVector2D Location, bool bSelectNewNode)
{
	const FScopedTransaction Transaction(NSLOCTEXT("GameFlowEditor", "CreateNode", "Create Node"));
	
	UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(ParentGraph);
	UGameFlowAsset* GameFlowAsset = GameFlowGraph->GameFlowAsset;
	
	// Tell the transaction system that these objects
	// will be modified inside this func scope.
	GameFlowAsset->Modify();
	ParentGraph->Modify();
	if(FromPin != nullptr)
	{
		FromPin->Modify();
	}

	// Create the actual graph node.
	UGameFlowGraphNode* GraphNode = CreateNode(NodeClass, GameFlowGraph, FromPin);
    // Position it at mouse click position.
	GraphNode->NodePosX = Location.X;
	GraphNode->NodePosY = Location.Y;
	return GraphNode;
}

UEdGraphNode* FGameFlowNodeSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins,
	const FVector2D Location, bool bSelectNewNode)
{
	return FEdGraphSchemaAction::PerformAction(ParentGraph, FromPins, Location, bSelectNewNode);
}

UGameFlowGraphNode* FGameFlowNodeSchemaAction_NewNode::CreateNode(UClass* NodeClass, UGameFlowGraph* GameFlowGraph,
	UEdGraphPin* FromPin)
{
	UGameFlowAsset* GameFlowAsset = GameFlowGraph->GameFlowAsset;
	const UGameFlowGraphSchema* GameFlowGraphSchema = CastChecked<UGameFlowGraphSchema>(GameFlowGraph->GetSchema());
	
	UGameFlowNode* NewNode = NewObject<UGameFlowNode>(GameFlowAsset, NodeClass, NAME_None, RF_Transactional);
	UGameFlowGraphNode* GraphNode = NewObject<UGameFlowGraphNode>(GameFlowGraph, NAME_None, RF_Transactional);
	
	if(FromPin != nullptr)
	{
		// Connect the dragged pin to the new graph node default pin.
		GameFlowGraphSchema->ConnectToDefaultPin(FromPin, GraphNode, GameFlowGraph);
	}

	// Initialize UedGraphNode properties.
	GraphNode->CreateNewGuid();
	
	// Create asset and respective graph node
	GraphNode->SetNodeAsset(NewNode);
	GraphNode->InitNode();
	
	// Add the graph node to the outer graph.
	GameFlowGraph->AddNode(GraphNode, false, false);
	
	// Once we've completed creation and initialization process,
	// notify graph node that it has successfully been placed
	// inside the graph.
	GraphNode->PostPlacedNewNode();
	
	return GraphNode;
}

UGameFlowGraphNode* FGameFlowNodeSchemaAction_NewNode::CreateNode(UGameFlowNode* NodeAsset,
	UGameFlowGraph* GameFlowGraph, UEdGraphPin* FromPin)
{
	const UGameFlowGraphSchema* GameFlowGraphSchema = CastChecked<UGameFlowGraphSchema>(GameFlowGraph->GetSchema());
	UGameFlowGraphNode* GraphNode = NewObject<UGameFlowGraphNode>(GameFlowGraph, NAME_None, RF_Transactional);

	if(FromPin != nullptr)
	{
		// Connect the dragged pin to the new graph node default pin.
		GameFlowGraphSchema->ConnectToDefaultPin(FromPin, GraphNode, GameFlowGraph);
	}
	
	// Initialize UedGraphNode properties.
	GraphNode->CreateNewGuid();
	
	// Create asset and respective graph node
	GraphNode->SetNodeAsset(NodeAsset);
	GraphNode->InitNode();
	
	// Add the graph node to the outer graph.
	GameFlowGraph->AddNode(GraphNode, false, false);
	
	// Once we've completed creation and initialization process,
	// notify graph node that it has successfully been placed
	// inside the graph.
	GraphNode->PostPlacedNewNode();
	
	return GraphNode;
}
