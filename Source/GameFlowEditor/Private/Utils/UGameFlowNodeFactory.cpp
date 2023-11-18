// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/UGameFlowNodeFactory.h"

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
	Factory.Finalize();
	
	// Initialize the node.
	Node->NodeAsset = NodeAsset;
	Node->InitNode();
	
	return Node;
}

UGameFlowGraphNode* UGameFlowNodeFactory::CreateGraphNode(const TSubclassOf<UGameFlowNode> NodeClass,
	UGameFlowAsset* ParentAsset, UGameFlowGraph* Graph)
{
	const FName NodeInstanceName = FName(ParentAsset->GetName() + "_GameFlowNode");
	// Create a brand new instance of node of supplied class.
	UGameFlowNode* NewNode = NewObject<UGameFlowNode>(ParentAsset, NodeClass, NodeInstanceName);
    ParentAsset->Nodes.Add(NewNode);
	
	// Create and return the graph node.
	return CreateGraphNode(NewNode, Graph);
}
