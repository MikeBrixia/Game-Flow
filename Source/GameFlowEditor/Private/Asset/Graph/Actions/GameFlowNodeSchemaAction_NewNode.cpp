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
	UGameFlowGraphNode* GraphNode = CreateNode(NodeClass, GameFlowGraph, Location, EName::None, FromPin);
	
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
   FVector2D Location, FName NodeName, UEdGraphPin* FromPin)
{
	UGameFlowAsset* GameFlowAsset = GameFlowGraph->GameFlowAsset;
	
	UGameFlowNode* NewNode = NewObject<UGameFlowNode>(GameFlowAsset, NodeClass, NodeName, RF_Transactional);
	UGameFlowGraphNode* NewGraphNode = CreateNode(NewNode, GameFlowGraph, Location, NodeName, FromPin);
	
	return NewGraphNode;
}

UGameFlowGraphNode* FGameFlowNodeSchemaAction_CreateOrDestroyNode::CreateNode(UGameFlowNode* NodeAsset,
	UGameFlowGraph* GameFlowGraph, FVector2D Location, FName NodeName, UEdGraphPin* FromPin)
{
	const FName FullNodeName = NodeName.IsNone()?  NodeName : FName("GameFlowGraphNode_" + NodeName.ToString());
	UGameFlowGraphNode* TemplateGraphNode = NewObject<UGameFlowGraphNode>(GameFlowGraph, FullNodeName, RF_Transactional);
	
	// Node asset must be set on the template node before creating the final
	// node with the schema action.
	TemplateGraphNode->SetNodeAsset(NodeAsset);
	TemplateGraphNode->CreateNewGuid();
	NodeAsset->GUID = TemplateGraphNode->NodeGuid;
	
	UEdGraphNode* NewNode = FEdGraphSchemaAction_NewNode::CreateNode(GameFlowGraph, FromPin, Location, TemplateGraphNode);
	// Workaround to ensure Graph node and logical node shares the same GUID
	// while also maintaining Unreal ed graph creation workflow.
	NewNode->NodeGuid = NodeAsset->GUID;
	return CastChecked<UGameFlowGraphNode>(NewNode);
}

UGameFlowGraphNode* FGameFlowNodeSchemaAction_CreateOrDestroyNode::RecreateNode(UGameFlowNode* NodeAsset,
	UGameFlowGraph* GameFlowGraph, FName NodeName)
{
	const FName FullNodeName = NodeName.IsNone()?  NodeName : FName("GameFlowGraphNode_" + NodeName.ToString());
	UGameFlowGraphNode* TemplateGraphNode = NewObject<UGameFlowGraphNode>(GameFlowGraph, FullNodeName, RF_Transactional);
	// Create asset and respective graph node
	TemplateGraphNode->SetNodeAsset(NodeAsset);
	
	UEdGraphNode* NewNode = FEdGraphSchemaAction_NewNode::CreateNode(GameFlowGraph, nullptr, NodeAsset->GraphPosition, TemplateGraphNode);
	// This ensures that we can find a given graph node by using the node asset GUID.
	NewNode->NodeGuid = NodeAsset->GUID;

	return CastChecked<UGameFlowGraphNode>(NewNode);;
}
