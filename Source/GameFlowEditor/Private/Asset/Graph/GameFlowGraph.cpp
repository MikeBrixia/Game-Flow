﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowGraph.h"
#include "GameFlowEditor.h"
#include "GraphEditAction.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Misc/ITransactionObjectAnnotation.h"
#include "Misc/TransactionObjectEvent.h"
#include "Utils/GameFlowEditorSubsystem.h"
#include "Utils/UGameFlowNodeFactory.h"

class UGameFlowNode;

UGameFlowGraph::UGameFlowGraph()
{
}

void UGameFlowGraph::InitGraph()
{
	const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	
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
		// Compilation callbacks.
		CompileCallback.AddUObject(this, &UGameFlowGraph::OnGraphCompile);

		// Cpp compilation callbacks, both for Live Coding and Hot Reload.
		FCoreUObjectDelegates::ReloadCompleteDelegate.AddUObject(this, &UGameFlowGraph::OnHotReload);
		FCoreUObjectDelegates::CompiledInUObjectsRegisteredDelegate.AddUObject(this, &UGameFlowGraph::OnLiveCompile);
		
		FOnAssetSaved& SaveCallback = Editor->GetAssetSavedCallback();
		SaveCallback.AddUObject(this, &UGameFlowGraph::OnSaveGraph);
	}
	else
	{
		UE_LOG(LogGameFlow, Warning, TEXT("Warning: %s Asset Editor could not be found! This may prevent graph from reacting to editor events/commands"),
			*GameFlowAsset->GetName());
	}
}

TArray<UGameFlowGraphNode*> UGameFlowGraph::GetNodesOfClass(const TSubclassOf<UGameFlowNode> NodeClass) const
{
	const TArray<UGameFlowGraphNode*> GameFlowGraphNodes = reinterpret_cast<const TArray<UGameFlowGraphNode*>&>(Nodes);
	// Find all graph nodes which encapsulates node assets of the requested type(NodeClass).
	TArray<UGameFlowGraphNode*> NodesOfRequestedType = GameFlowGraphNodes.FilterByPredicate(
		[=](const UGameFlowGraphNode* Node)
		{
			return Node->GetNodeAsset()->GetClass() == NodeClass;
		});
	return NodesOfRequestedType;
}

UGameFlowGraphNode* UGameFlowGraph::GetGraphNodeByAsset(const UGameFlowNode* NodeAsset) const
{
	TArray<UGameFlowGraphNode*> GraphNodes = reinterpret_cast<const TArray<UGameFlowGraphNode*>&>(Nodes);
	// Find the graph node which encapsulates NodeAsset.
	UGameFlowGraphNode** NodeRef = GraphNodes.FindByPredicate(
		[=](const UGameFlowGraphNode* GraphNode)
	    {
			return NodeAsset == GraphNode->GetNodeAsset();
	    });
	return NodeRef != nullptr? *NodeRef : nullptr;
}

TArray<UGameFlowGraphNode*> UGameFlowGraph::GetOrphanNodes() const
{
	TArray<UGameFlowGraphNode*> OrphanNodes;
	// Find orphan nodes.
	for(UGameFlowGraphNode* GraphNode : reinterpret_cast<const TArray<UGameFlowGraphNode*>&>(Nodes))
	{
		if(GraphNode->IsOrphan())
		{
			OrphanNodes.Add(GraphNode);
		}
	}
    
	return OrphanNodes;
}

void UGameFlowGraph::OnGraphCompile()
{
	const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	checkf(GraphSchema != nullptr, TEXT("Game Flow Graph Schema is invalid! Please assign a valid schema to this graph"));

	GraphSchema->ValidateAsset(*this);
	const bool bCompilationSuccessful = GraphSchema->CompileGraph(*this, GameFlowAsset);
	const FString AssetName = GameFlowAsset->GetName();
	
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

#if WITH_HOT_RELOAD

void UGameFlowGraph::OnHotReload(EReloadCompleteReason ReloadCompleteReason)
{
	const TArray<UGameFlowGraphNode*> ReloadedNodes = reinterpret_cast<const TArray<UGameFlowGraphNode*>&>(Nodes);
	for(UGameFlowGraphNode* Node : ReloadedNodes)
	{
		Node->OnLiveOrHotReloadCompile();
	}
}

#endif

#if WITH_LIVE_CODING

void UGameFlowGraph::OnLiveCompile(const TArray<UClass*>& ReloadedClasses)
{
	UE_LOG(LogGameFlow, Display, TEXT("Live Compile"))
}

void UGameFlowGraph::OnLiveCompile(FName Name)
{
	const TArray<UGameFlowGraphNode*> ReloadedNodes = reinterpret_cast<const TArray<UGameFlowGraphNode*>&>(Nodes);
	for(UGameFlowGraphNode* Node : ReloadedNodes)
	{
		Node->OnLiveOrHotReloadCompile();
	}
}

#endif

void UGameFlowGraph::NotifyGraphChanged(const FEdGraphEditAction& Action)
{
	Super::NotifyGraphChanged(Action);
	const TArray<UGameFlowGraphNode*> ModifiedNodes = reinterpret_cast<const TArray<UGameFlowGraphNode*>&>(Action.Nodes);
	
	// We want to use a set of UGameFlowGraphNode type.
	switch(Action.Action)
	{
		case GRAPHACTION_SelectNode:
			{
				OnNodesSelected(ModifiedNodes);
				break;
			}
	    case GRAPHACTION_AddNode:
			{
				OnNodesAdded(ModifiedNodes);
				break;
			}
	    case GRAPHACTION_RemoveNode:
			{
				OnNodesRemoved(ModifiedNodes);
				break;
			}
		
		default: break;
	}
}

void UGameFlowGraph::OnNodesSelected(const TArray<UGameFlowGraphNode*> SelectedNodes)
{
	// Array of selected nodes assets.
	TArray<UObject*> SelectedAssets;
                
	// Build selected nodes assets array.
	for(const UGameFlowGraphNode* SelectedNode : SelectedNodes)
	{
		UGameFlowNode* NodeAsset = SelectedNode->GetNodeAsset();
		SelectedAssets.Add(NodeAsset);
	}
				
	// Inspect selected nodes inside editor nodes details view.
	GameFlowEditor->NodesDetailsView->SetObjects(SelectedAssets);
}

void UGameFlowGraph::OnNodesRemoved(const TArray<UGameFlowGraphNode*>& RemovedNodes)
{
	for(const UGameFlowGraphNode* GraphNode : RemovedNodes)
	{
		// Remove node from the game flow asset.
		GameFlowAsset->Nodes.Remove(GraphNode->GetNodeAsset());
	}
}

void UGameFlowGraph::OnNodesAdded(const TArray<UGameFlowGraphNode*>& AddedNodes)
{
	for(const UGameFlowGraphNode* GraphNode : AddedNodes)
	{
		// Add node to the game flow asset.
		GameFlowAsset->Nodes.AddUnique(GraphNode->GetNodeAsset());
	}
}

void UGameFlowGraph::RebuildGraphFromAsset()
{
	const UGameFlowGraphSchema* GameFlowSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	// Recreate all game flow asset registered nodes, including orphan nodes.
	// Orphans are nodes which does not share connections with any parent node,
	// e.g. their input pins have no links.
	for(UGameFlowNode* NodeAsset : GameFlowAsset->Nodes)
	{
		UGameFlowNodeFactory::CreateGraphNode(NodeAsset, this);
	}
	
	// Recreate all graph node connections.
	GameFlowSchema->RecreateGraphNodesConnections(*this);
}





