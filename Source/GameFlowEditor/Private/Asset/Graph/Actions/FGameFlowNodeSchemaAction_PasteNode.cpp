#include "Asset/Graph/Actions/FGameFlowNodeSchemaAction_PasteNode.h"
#include "Asset/Graph/GameFlowGraph.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"

UEdGraphNode* FGameFlowNodeSchemaAction_PasteNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                                 const FVector2D Location, bool bSelectNewNode)
{
	UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(ParentGraph);
	GameFlowGraph->Modify();
	
	UGameFlowGraphNode* PastedGraphNode = CastChecked<UGameFlowGraphNode>(
		StaticDuplicateObject(NodeToPaste, GameFlowGraph));
	PastedGraphNode->NodePosX = Location.X;
	PastedGraphNode->NodePosY = Location.Y;
	
	// Add the graph node to the outer graph.
	GameFlowGraph->AddNode(PastedGraphNode, false, false);
	PastedGraphNode->PostPlacedNewNode();
	PastedGraphNode->CreateNewGuid();
	
	return PastedGraphNode;
}

UEdGraphNode* FGameFlowNodeSchemaAction_PasteNode::PerformAction(UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins,
	const FVector2D Location, bool bSelectNewNode)
{
	return FEdGraphSchemaAction::PerformAction(ParentGraph, FromPins, Location, bSelectNewNode);
}
