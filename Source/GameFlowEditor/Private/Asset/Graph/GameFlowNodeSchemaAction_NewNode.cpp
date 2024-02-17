// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowNodeSchemaAction_NewNode.h"
#include "Asset/Graph/GameFlowGraphSchema.h"

UEdGraphNode* FGameFlowNodeSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                               const FVector2D Location, bool bSelectNewNode)
{
	const FScopedTransaction Transaction(NSLOCTEXT("GameFlowEditor", "CreateNode", "Create Node"));
	
	UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(ParentGraph);
	UGameFlowAsset* GameFlowAsset = GameFlowGraph->GameFlowAsset;
	
	UGameFlowNode* NewNode = NewObject<UGameFlowNode>(GameFlowAsset, NodeClass, NAME_None, RF_Transactional);
	UGameFlowGraphNode* GraphNode = NewObject<UGameFlowGraphNode>(ParentGraph, NAME_None, RF_Transactional);
	
	// Tell the transaction system that these objects
	// will be modified inside this func scope.
	GameFlowAsset->Modify();
	GameFlowGraph->Modify();
	if(FromPin != nullptr)
	{
		FromPin->Modify();
		// Connect the dragged pin to the new graph node default pin.
		ConnectToDefaultPin(FromPin, GraphNode, GameFlowGraph);
	}
	
	// Initialize UedGraphNode properties.
	GraphNode->CreateNewGuid();
	GraphNode->NodePosX = Location.X;
	GraphNode->NodePosY = Location.Y;
	
	// Create asset and respective graph node
	GraphNode->SetNodeAsset(NewNode);
	GraphNode->InitNode();

	// Add the graph node to the outer graph.
	GameFlowGraph->AddNode(GraphNode, false, bSelectNewNode);
	
	// Once we've completed creation and initialization process,
	// notify graph node that it has successfully been placed
	// inside the graph.
	GraphNode->PostPlacedNewNode();
	
	return GraphNode;
}

UEdGraphNode* FGameFlowNodeSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins,
	const FVector2D Location, bool bSelectNewNode)
{
	return FEdGraphSchemaAction::PerformAction(ParentGraph, FromPins, Location, bSelectNewNode);
}

void FGameFlowNodeSchemaAction_NewNode::ConnectToDefaultPin(UEdGraphPin* FromPin, UEdGraphNode* GraphNode, const UGameFlowGraph* Graph) const
{
	UEdGraphPin* NewNodeTargetPin = nullptr;
	// Depending on the direction, find a different target pin.
	if (FromPin->Direction == EGPD_Input)
	{
		NewNodeTargetPin = GraphNode->GetPinWithDirectionAt(0, EGPD_Output);
	}
	else if (FromPin->Direction == EGPD_Output)
	{
		NewNodeTargetPin = GraphNode->GetPinWithDirectionAt(0, EGPD_Input);
	}
	
	const UGameFlowGraphSchema* GameFlowGraphSchema = CastChecked<UGameFlowGraphSchema>(Graph->GetSchema());
	// Create a connection with the new node first pin.
	GameFlowGraphSchema->TryCreateConnection(FromPin, NewNodeTargetPin);
}
