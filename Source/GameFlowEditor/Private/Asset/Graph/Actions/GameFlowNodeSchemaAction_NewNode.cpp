// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/Actions/GameFlowNodeSchemaAction_NewNode.h"

#include "ScopedTransaction.h"
#include "Asset/Graph/GameFlowGraphSchema.h"

UEdGraphNode* FGameFlowNodeSchemaAction_CreateOrDestroyNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
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
	UGameFlowGraphNode* GraphNode = CreateNode(NodeClass, GameFlowGraph, EName::None, FromPin);
    // Position it at mouse click position.
	GraphNode->NodePosX = Location.X;
	GraphNode->NodePosY = Location.Y;
	return GraphNode;
}

UEdGraphNode* FGameFlowNodeSchemaAction_CreateOrDestroyNode::PerformAction(UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins,
	const FVector2D Location, bool bSelectNewNode)
{
	return FEdGraphSchemaAction::PerformAction(ParentGraph, FromPins, Location, bSelectNewNode);
}

void FGameFlowNodeSchemaAction_CreateOrDestroyNode::PerformAction_DestroyNode(UGameFlowGraphNode* GraphNode)
{
	FScopedTransaction Transaction(NSLOCTEXT("GameFlowEditor", "DestroyNode", "Destroy Node"));
	UGameFlowGraph* ParentGraph = CastChecked<UGameFlowGraph>(GraphNode->GetGraph());
	UGameFlowAsset* GameFlowAsset = ParentGraph->GameFlowAsset;
	
    // Record changes on game flow graph and asset.
	ParentGraph->Modify();
	ParentGraph->GameFlowAsset->Modify();
	GraphNode->DestroyNode();
	
	const UGameFlowNode* NodeAsset = GraphNode->GetNodeAsset();
	if(NodeAsset->IsA(UGameFlowNode_Input::StaticClass()))
	{
		GameFlowAsset->CustomInputs.Remove(NodeAsset->GetFName());
	}
	else if(NodeAsset->IsA(UGameFlowNode_Output::StaticClass()))
	{
		GameFlowAsset->CustomOutputs.Remove(NodeAsset->GetFName());
	}
}

UGameFlowGraphNode* FGameFlowNodeSchemaAction_CreateOrDestroyNode::CreateNode(UClass* NodeClass, UGameFlowGraph* GameFlowGraph,
                                                                  FName NodeName, UEdGraphPin* FromPin)
{
	UGameFlowAsset* GameFlowAsset = GameFlowGraph->GameFlowAsset;
	const UGameFlowGraphSchema* GameFlowGraphSchema = CastChecked<UGameFlowGraphSchema>(GameFlowGraph->GetSchema());
	
	UGameFlowNode* NewNode = NewObject<UGameFlowNode>(GameFlowAsset, NodeClass, NodeName, RF_Transactional);

	UGameFlowGraphNode* GraphNode = NewObject<UGameFlowGraphNode>(GameFlowGraph, NAME_None, RF_Transactional);
	if(FromPin != nullptr)
	{
		// Connect the dragged pin to the new graph node default pin.
		GameFlowGraphSchema->ConnectToDefaultPin(FromPin, GraphNode);
	}
	
	// Initialize GUID.
	GraphNode->CreateNewGuid();
	NewNode->GUID = GraphNode->NodeGuid;
	
	if(NewNode->IsA(UGameFlowNode_Input::StaticClass()))
	{
		// Register input node inside game flow asset.
		GameFlowAsset->CustomInputs.Add(NewNode->GetFName(), CastChecked<UGameFlowNode_Input>(NewNode));
	}
	else if(NewNode->IsA(UGameFlowNode_Output::StaticClass()))
	{
		// Register output node inside game flow asset.
		GameFlowAsset->CustomOutputs.Add(NewNode->GetFName(), CastChecked<UGameFlowNode_Output>(NewNode));
	}
	GameFlowAsset->AddNode(NewNode);
	
	// Assign node asset to graph node.
	GraphNode->SetNodeAsset(NewNode);
	
	// Add the graph node to the outer graph.
	GameFlowGraph->AddNode(GraphNode, false, false);
	
	// Once we've completed creation and initialization process,
	// notify graph node that it has successfully been placed
	// inside the graph.
	GraphNode->PostPlacedNewNode();
	
	return GraphNode;
}

UGameFlowGraphNode* FGameFlowNodeSchemaAction_CreateOrDestroyNode::CreateNode(UGameFlowNode* NodeAsset,
	UGameFlowGraph* GameFlowGraph, FName NodeName, UEdGraphPin* FromPin)
{
	const UGameFlowGraphSchema* GameFlowGraphSchema = CastChecked<UGameFlowGraphSchema>(GameFlowGraph->GetSchema());
	UGameFlowGraphNode* GraphNode = NewObject<UGameFlowGraphNode>(GameFlowGraph, NodeName, RF_Transactional);

	if(FromPin != nullptr)
	{
		// Connect the dragged pin to the new graph node default pin.
		GameFlowGraphSchema->ConnectToDefaultPin(FromPin, GraphNode);
	}
	
	// Initialize UedGraphNode properties.
	GraphNode->CreateNewGuid();
	
	// Create asset and respective graph node
	GraphNode->SetNodeAsset(NodeAsset);
	
	// Add the graph node to the outer graph.
	GameFlowGraph->AddNode(GraphNode, false, false);
	
	// Once we've completed creation and initialization process,
	// notify graph node that it has successfully been placed
	// inside the graph.
	GraphNode->PostPlacedNewNode();
	
	return GraphNode;
}

UGameFlowGraphNode* FGameFlowNodeSchemaAction_CreateOrDestroyNode::RecreateNode(UGameFlowNode* NodeAsset,
	UGameFlowGraph* GameFlowGraph, FName NodeName, UEdGraphPin* FromPin)
{
	UGameFlowGraphNode* GraphNode = NewObject<UGameFlowGraphNode>(GameFlowGraph, NodeName, RF_Transactional);
	// This ensures that we can find a given graph node by using
	// the node asset GUID.
	GraphNode->NodeGuid = NodeAsset->GUID;

	// Create asset and respective graph node
	GraphNode->SetNodeAsset(NodeAsset);
	
	// Add the graph node to the outer graph.
	GameFlowGraph->AddNode(GraphNode, false, false);
	
	// Once we've completed creation and initialization process,
	// notify graph node that it has successfully been placed
	// inside the graph.
	GraphNode->PostPlacedNewNode();

	return GraphNode;
}
