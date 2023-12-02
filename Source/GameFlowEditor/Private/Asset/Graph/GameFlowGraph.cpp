// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowGraph.h"
#include "GameFlowEditor.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Utils/UGameFlowNodeFactory.h"

class UGameFlowNode;

UGameFlowGraph::UGameFlowGraph()
{
}

void UGameFlowGraph::InitGraph()
{
	const UEdGraphSchema* GraphSchema = GetSchema();

	// Create default nodes only on first-time asset editor opening.
	if(!GameFlowAsset->bHasAlreadyBeenOpened)
    {
		UE_LOG(LogGameFlow, Display, TEXT("Create default nodes"))
		// Create the default nodes which will appear in the graph
		// when the graph editor gets opened.
		GraphSchema->CreateDefaultNodesForGraph(*this);
    }
	else
	{
		UE_LOG(LogGameFlow, Display, TEXT("Rebuild"))
		RebuildGraphFromAsset();
	}
}

void UGameFlowGraph::CompileGraph(UGameFlowAsset* Asset)
{
	// Compile graph starting from each input node.
	for(UGameFlowGraphNode* Root : RootNodes)
	{
		CompileInputNode(Root);
	}

	// Make sure compilation has been completed without any error.
	// In case there was an error, try to fix it.
	Asset->ValidateAsset();
}

void UGameFlowGraph::CompileInputNode(UGameFlowGraphNode* InputNode)
{
	// Start compiling from a graph input node.
	TQueue<UGameFlowGraphNode*> ToCompile;
	ToCompile.Enqueue(InputNode);

	// Keep going until we've compiled all graph nodes.
	while(!ToCompile.IsEmpty())
	{
		// Pick a new node to compile
		UGameFlowGraphNode* CurrentNode = nullptr;
		ToCompile.Dequeue(CurrentNode);
		
		for(const UEdGraphPin* Pin : CurrentNode->Pins)
		{
			// Check the links for all output pins.
			if(Pin->Direction == EGPD_Output)
			{
				UGameFlowNode* SourceNode = CurrentNode->GetNodeAsset();
				UGameFlowGraphNode* DestinationNode = CastChecked<UGameFlowGraphNode>(Pin->LinkedTo[0]->GetOwningNode());
				
				// Update node asset with graph connections.
				auto Outputs = SourceNode->GetOutputPins();
				SourceNode->AddOutput(Pin->PinName, DestinationNode->GetNodeAsset());
				
				// Put the destination node inside the queue, it's the next
				// we're going to compile.
				ToCompile.Enqueue(DestinationNode);
			}
		}
	}
}

void UGameFlowGraph::RebuildGraphFromAsset()
{
	// Recreate all nodes by reading the game flow asset data.
	for(const auto Pair : GameFlowAsset->CustomInputs)
	{
		UGameFlowNode* NodeAsset = nullptr;
		UGameFlowGraphNode* CurrentNode = nullptr;
		UGameFlowGraphNode* PreviousNode = nullptr;
		
		TQueue<UGameFlowNode*> ToRebuild;
        ToRebuild.Enqueue(Pair.Value);
		
		while(!ToRebuild.IsEmpty())
		{
			// Extract the next node asset to rebuild.
			ToRebuild.Dequeue(NodeAsset);

			// Initialize previous node and create a brand new graph node.
			PreviousNode = CurrentNode;
			CurrentNode = UGameFlowNodeFactory::CreateGraphNode(NodeAsset, this);
			Nodes.Add(CurrentNode);
			
			for(const FName& PinName : NodeAsset->GetOutputPins())
			{
				UGameFlowNode* NextNode = NodeAsset->GetNextNode(PinName);
				ToRebuild.Enqueue(NextNode);
			}
			
			if(PreviousNode != nullptr)
			{
			}
		}
	}
    
	// Recreate all links starting from game flow asset root nodes.
}

void UGameFlowGraph::NotifyGraphChanged()
{
	Super::NotifyGraphChanged();
}

