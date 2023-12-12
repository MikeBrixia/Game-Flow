// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowGraph.h"

#include "GameFlowEditor.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Utils/GameFlowEditorSubsystem.h"
#include "Utils/UGameFlowNodeFactory.h"

class UGameFlowNode;

UGameFlowGraph::UGameFlowGraph()
{
}

void UGameFlowGraph::InitGraph()
{
	const UEdGraphSchema* GraphSchema = GetSchema();
	
	const UGameFlowEditorSubsystem* GameFlowEditorSubsystem = GEditor->GetEditorSubsystem<UGameFlowEditorSubsystem>();
	GameFlowEditor = GameFlowEditorSubsystem->GetActiveEditorByAssetName(GameFlowAsset->GetFName());
	// Register to Game Flow editor commands.
	SubscribeToEditorCallbacks(GameFlowEditor);
	
	// Create default nodes only on first-time asset editor opening.
	if(!GameFlowAsset->bHasAlreadyBeenOpened)
    {
		// Create the default nodes which will appear in the graph
		// when the graph editor gets opened.
		GraphSchema->CreateDefaultNodesForGraph(*this);
    }
	else
	{
		RebuildGraphFromAsset();
	}
}

void UGameFlowGraph::SubscribeToEditorCallbacks(GameFlowAssetToolkit* Editor)
{
	if(Editor != nullptr)
	{
		FOnAssetCompile& CompileCallback = Editor->GetAssetCompileCallback();
		CompileCallback.AddUObject(this, &UGameFlowGraph::OnGraphCompile);
    
		FOnAssetSaved& SaveCallback = Editor->GetAssetSavedCallback();
		SaveCallback.AddUObject(this, &UGameFlowGraph::OnSaveGraph);
	}
	else
	{
		UE_LOG(LogGameFlow, Warning, TEXT("Warning: %s Asset Editor could not be found! This may prevent graph from reacting to editor events/commands"),
			*GameFlowAsset->GetName())
	}
}

void UGameFlowGraph::OnGraphCompile(UGameFlowAsset* Asset)
{
	CompileGraph();
}

void UGameFlowGraph::OnSaveGraph()
{
	UE_LOG(LogGameFlow, Warning, TEXT("%s: Saving has not yet been implemented!"), *StaticClass()->GetName());
}

bool UGameFlowGraph::CompileGraph()
{
	for(UGameFlowGraphNode* Node : RootNodes)
	{
		UE_LOG(LogGameFlow, Display, TEXT("Compiling root: %s"), *Node->GetName())
		CompileGraphFromInputNode(Node);
	}
	return true;
}

bool UGameFlowGraph::CompileGraphFromInputNode(UGameFlowGraphNode* InputNode)
{
	bool bIsCompilationSuccessful = true;
	
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
			// If this pin does not have any connection, ignore it.
			if(!Pin->HasAnyConnections()) continue;
			
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
	return bIsCompilationSuccessful;
}

void UGameFlowGraph::RebuildGraphFromAsset()
{
	for(const auto Pair : GameFlowAsset->Nodes)
	{
		UGameFlowNode* Node = Pair.Value;
		if(Pair.Value != nullptr)
		{
			UGameFlowGraphNode* GraphNode = UGameFlowNodeFactory::CreateGraphNode(Node, this);
			if(Node->IsA(UGameFlowNode_Input::StaticClass()))
			{
				UE_LOG(LogGameFlow, Display, TEXT("%s is a Input node"), *Node->GetName())
				RootNodes.Add(GraphNode);
			}
		}
	}
}

void UGameFlowGraph::NotifyGraphChanged()
{
	Super::NotifyGraphChanged();
}

void UGameFlowGraph::OnSelectionChanged(const TSet<UObject*>& Selection)
{
	// Array of GameFlow assets contained inside the selected graph nodes.
	TArray<UObject*> SelectedNodes;

	// Notify Graph nodes that they have been selected.
	for(UObject* SelectedObject : Selection)
	{
		UGameFlowGraphNode* GraphNode = CastChecked<UGameFlowGraphNode>(SelectedObject);
		GraphNode->OnSelected();
		SelectedNodes.Add(GraphNode->GetNodeAsset());
	}

	// Inspect selected nodes inside editor nodes details view.
	GameFlowEditor->NodesDetailsView->SetObjects(SelectedNodes);
}


