// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowGraph.h"
#include "GameFlowEditor.h"
#include "GraphEditAction.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Asset/Graph/Actions/GameFlowNodeSchemaAction_NewNode.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Utils/GameFlowEditorSubsystem.h"
#include "Widget/SGameFlowReplaceNodeDialog.h"

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
	const TSet<UGameFlowGraphNode*> ModifiedNodes = reinterpret_cast<const TSet<UGameFlowGraphNode*>&>(Action.Nodes);
	
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

void UGameFlowGraph::OnDummyReplacementRequest()
{
	const TSharedRef<SGameFlowReplaceNodeDialog> ReplaceNodeDialog = SNew(SGameFlowReplaceNodeDialog);
	const int32 PressedButtonIndex = ReplaceNodeDialog->ShowModal();
	UClass* PickedClass = ReplaceNodeDialog->GetPickedClass();
	
	// Has the user picked a valid replacement class and clicked the "Replace" button?
	if(PressedButtonIndex == 0 && PickedClass != nullptr
		&& ReplaceNodeDialog->ShouldReplaceAll())
	{
		TArray<UGameFlowGraphNode*> GraphNodes = GetNodesOfClass(PickedClass);
		for(UGameFlowGraphNode* NodeToReplace : GraphNodes)
		{
			ReplaceGraphNode(NodeToReplace, PickedClass);
		}
	}
	
}

void UGameFlowGraph::ReplaceGraphNode(UGameFlowGraphNode* NodeToReplace, UClass* ReplacementClass) 
{
	UGameFlowNode* NodeAsset = NodeToReplace->GetNodeAsset();
	const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());

	UGameFlowNode* SubstituteNodeAsset = NewObject<UGameFlowNode>(GameFlowAsset, ReplacementClass, NAME_None,
	                                                              RF_Transactional);
	FObjectInstancingGraph ObjectInstancingGraph;
	ObjectInstancingGraph.AddNewObject(SubstituteNodeAsset, NodeAsset);
	TSet<FName> InOutExtraNames;
	UGameFlowGraphNode* SubstituteNode = CastChecked<UGameFlowGraphNode>(
		GraphSchema->CreateSubstituteNode(NodeToReplace, this, &ObjectInstancingGraph, InOutExtraNames)
	);
	FGameFlowNodeSchemaAction_CreateOrDestroyNode DestroyNodeAction;
	DestroyNodeAction.PerformAction_DestroyNode(NodeToReplace);
}

void UGameFlowGraph::OnNodesSelected(const TSet<UGameFlowGraphNode*> SelectedNodes)
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

void UGameFlowGraph::OnNodesRemoved(const TSet<UGameFlowGraphNode*> RemovedNodes)
{
	for(UGameFlowGraphNode* GraphNode : RemovedNodes)
	{
		// Remove node from the game flow asset.
		UGameFlowNode* NodeAsset = GraphNode->GetNodeAsset();
		GameFlowAsset->Nodes.Remove(NodeAsset);
	}
}

void UGameFlowGraph::OnNodesAdded(const TSet<UGameFlowGraphNode*> AddedNodes)
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
		FGameFlowNodeSchemaAction_CreateOrDestroyNode::CreateNode(NodeAsset, this);
	}
	 
	// Recreate all graph node connections.
	GameFlowSchema->RecreateGraphNodesConnections(*this);
}





