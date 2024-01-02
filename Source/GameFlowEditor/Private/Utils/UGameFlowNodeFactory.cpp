// Fill out your copyright notice in the Description page of Project Settings.

#include "Utils/UGameFlowNodeFactory.h"

#include "Asset/Graph/GameFlowGraphSchema.h"

UObject* UGameFlowNodeFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	return NewObject<UGameFlowNode>(InParent, InClass, InName, Flags, Context);
}

UGameFlowGraphNode* UGameFlowNodeFactory::CreateGraphNode(UGameFlowNode* NodeAsset, UGameFlowGraph* Graph)
{
	// Make sure both node class and parent graph are valid.
	checkf(Graph, TEXT("Invalid parent graph! Nodes must be created inside a valid graph(not nullptr)."));

	// Create the node
	FGraphNodeCreator<UGameFlowGraphNode> Factory {*Graph};
	UGameFlowGraphNode* Node = Factory.CreateNode(false);
    
	// Initialize the node.
	Node->NodeAsset = NodeAsset;
	Node->NodePosX = NodeAsset->GraphPosition.X;
	Node->NodePosY = NodeAsset->GraphPosition.Y;
	Node->InitNode();

	// Call this to end node construction.
	Factory.Finalize();
	
	return Node;
}

UGameFlowGraphNode* UGameFlowNodeFactory::CreateGraphNode(const TSubclassOf<UGameFlowNode> NodeClass,
	UGameFlowAsset* ParentAsset, UGameFlowGraph* Graph)
{
	// Create a brand new instance of node of supplied class.
	UGameFlowNode* NewNode = CreateGameFlowNode(NodeClass, ParentAsset);
	
	// Create and return the graph node.
	return CreateGraphNode(NewNode, Graph);
}

UGameFlowNode* UGameFlowNodeFactory::CreateGameFlowNode(const TSubclassOf<UGameFlowNode> NodeClass,
	UGameFlowAsset* GameFlowAsset)
{
	// Create a brand new instance of node of supplied class.
	UGameFlowNode* NewNode = NewObject<UGameFlowNode>(GameFlowAsset, NodeClass);
	GameFlowAsset->Nodes.Add(NewNode->GetUniqueID(), NewNode);

	return NewNode;
}
