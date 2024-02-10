
// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowNodeSchemaAction_NewNode.h"

#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Utils/UGameFlowNodeFactory.h"

UEdGraphNode* FGameFlowNodeSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                               const FVector2D Location, bool bSelectNewNode)
{
	UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(ParentGraph);
	// Create a brand new game flow graph node of supplied type.
	UGameFlowGraphNode* GraphNode = UGameFlowNodeFactory::CreateGraphNode(NodeClass, GameFlowGraph->GameFlowAsset, GameFlowGraph);
	// Initialize UedGraphNode properties.
	GraphNode->NodePosX = Location.X;
	GraphNode->NodePosY = Location.Y;
	GraphNode->Modify();
	GraphNode->PostPlacedNewNode();
	GraphNode->CreateNewGuid();
	
	// Is the context menu been created after a pin drag action?
	if(FromPin != nullptr)
	{
		UEdGraphPin* NewNodeTargetPin = nullptr;
        if(FromPin->Direction == EGPD_Input)
        {
	        NewNodeTargetPin = GraphNode->GetPinWithDirectionAt(0, EGPD_Output);
        }
		else if(FromPin->Direction == EGPD_Output)
		{
			NewNodeTargetPin = GraphNode->GetPinWithDirectionAt(0, EGPD_Input);
		}
		const UGameFlowGraphSchema* GameFlowGraphSchema = CastChecked<UGameFlowGraphSchema>(GameFlowGraph->GetSchema());
		// If true create an automatic connection with the new node first pin.
		GameFlowGraphSchema->TryCreateConnection(FromPin, NewNodeTargetPin);
	}
	
	return GraphNode;
}

UEdGraphNode* FGameFlowNodeSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins,
	const FVector2D Location, bool bSelectNewNode)
{
	return FEdGraphSchemaAction::PerformAction(ParentGraph, FromPins, Location, bSelectNewNode);
}

