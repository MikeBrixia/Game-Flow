// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowGraph.h"

#include "GameFlowEditor.h"
#include "GraphEditAction.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
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
			*GameFlowAsset->GetName());
	}
}

void UGameFlowGraph::OnGraphCompile(UGameFlowAsset* Asset)
{
	const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	checkf(GraphSchema != nullptr, TEXT("Game Flow Graph Schema is invalid! Please assign a valid schema to this graph"));

	const bool bCompilationSuccessful = GraphSchema->CompileGraph(*this, Asset);
	const FString AssetName = Asset->GetName();
	
	// Log the result of the graph compilation to the Unreal engine output log and
	// also the GameFlow log console.
	if(bCompilationSuccessful)
	{
		UE_LOG(LogGameFlow, Display, TEXT("Game Flow Asset: %s, was compiled successfully"), *AssetName);
	}
	else
	{
		UE_LOG(LogGameFlow, Error, TEXT("Game Flow Asset: %s, could not be compiled"), *AssetName);
	}
}

void UGameFlowGraph::OnSaveGraph()
{
	for(const UGameFlowGraphNode* GraphNode : reinterpret_cast<TArray<TObjectPtr<UGameFlowGraphNode>>&>(Nodes))
	{
		UGameFlowNode* NodeAsset = GraphNode->GetNodeAsset();
		// Save node position inside the graph, will be used when graph rebuild happens.
		NodeAsset->GraphPosition = FVector2D(GraphNode->NodePosX, GraphNode->NodePosY);
	}
}

void UGameFlowGraph::NotifyGraphChanged(const FEdGraphEditAction& Action)
{
	Super::NotifyGraphChanged(Action);
	
	switch(Action.Action)
	{
		case GRAPHACTION_SelectNode:
			{
				// Array of selected nodes assets.
				TArray<UObject*> SelectedNodes;

				// Build selected nodes assets array.
				for(const UObject* SelectedObject : Action.Nodes)
				{
					const UGameFlowGraphNode* GraphNode = CastChecked<UGameFlowGraphNode>(SelectedObject);
					SelectedNodes.Add(GraphNode->GetNodeAsset());
				}
		        
				// Inspect selected nodes inside editor nodes details view.
				GameFlowEditor->NodesDetailsView->SetObjects(SelectedNodes);
				break;
			}
            
		case GRAPHACTION_RemoveNode:
			{
				const TSet<const UGameFlowGraphNode*> RemovedNodes = reinterpret_cast<const TSet<const UGameFlowGraphNode*>&>(Action.Nodes);
				for(const UGameFlowGraphNode* RemovedNode : RemovedNodes)
				{
					const UGameFlowNode* NodeAsset = RemovedNode->GetNodeAsset();
					GameFlowAsset->Nodes.Remove(NodeAsset->GetUniqueID());
				}
				break;
			}
             
		default: break;
	}
}

void UGameFlowGraph::RebuildGraphFromAsset()
{
	// Recreate all game flow asset registered nodes, including orphan nodes.
	// Orphans are nodes which does not share connections with any parent node,
	// e.g. their input pins have no links.
	for(const auto& Pair : GameFlowAsset->Nodes)
	{
		UGameFlowNode* Node = Pair.Value;
		UGameFlowGraphNode* GraphNode = UGameFlowNodeFactory::CreateGraphNode(Node, this);
		GraphNodes.Add(Node->GetUniqueID(), GraphNode);
		
		// if  node asset is an input node, add it's graph node representation to the root
		// nodes array list for this graph.
		if(Node->IsA(UGameFlowNode_Input::StaticClass()))
		{
			RootNodes.Add(GraphNode);
		}
	}

	const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	// Recreate node connections starting from each root.
	for(const auto PinNameNodePair : GameFlowAsset->CustomInputs)
	{
		GraphSchema->RecreateBranchConnections(*this, PinNameNodePair.Value);
	}
}





