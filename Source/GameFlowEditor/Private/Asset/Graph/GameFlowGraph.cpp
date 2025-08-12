// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowGraph.h"
#include "GameFlowEditor.h"
#include "GraphEditAction.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Asset/Graph/Actions/FGameFlowSchemaAction_ReplaceNode.h"
#include "Asset/Graph/Actions/GameFlowNodeSchemaAction_NewNode.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Widget/SGameFlowReplaceNodeDialog.h"

class UGameFlowNode;

UGameFlowGraph::UGameFlowGraph()
{
	this->DebuggedAssetInstance = nullptr;
	this->GameFlowAsset = nullptr;
}

void UGameFlowGraph::InitGraph()
{
	const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	
	// Listen to editor events.
	FCoreUObjectDelegates::ReloadCompleteDelegate.AddUObject(this, &UGameFlowGraph::OnHotReload);
	FCoreUObjectDelegates::CompiledInUObjectsRegisteredDelegate.AddUObject(this, &UGameFlowGraph::OnLiveCompile);
	
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

void UGameFlowGraph::SetDebuggedInstance(UGameFlowAsset* Instance)
{
	// Only instances of the same type as GameFlowAsset can be debugged inside the graph.
	// Let pass invalid instances.
	if(Instance == nullptr || Instance->IsA(GameFlowAsset->GetClass()))
	{
		// Finally set the new debugged instance and broadcast the event.
		this->DebuggedAssetInstance = Instance;
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

TArray<UGameFlowGraphNode*> UGameFlowGraph::GetRootNodes() const
{
	TArray<UGameFlowGraphNode*> RootNodes;
	const TArray<UGameFlowGraphNode*>& GraphNodes = reinterpret_cast<const TArray<UGameFlowGraphNode*>&>(Nodes);

	// Create an array of only root nodes and return it.
	return GraphNodes.FilterByPredicate([] (const UGameFlowGraphNode* GraphNode)
	{
		return GraphNode->IsRoot();
	});
}

TArray<UGameFlowGraphNode*> UGameFlowGraph::GetActiveNodes() const
{
	TArray<UGameFlowGraphNode*> RootNodes;
	const TArray<UGameFlowGraphNode*>& GraphNodes = reinterpret_cast<const TArray<UGameFlowGraphNode*>&>(Nodes);

	// Create an array of only root nodes and return it.
	return GraphNodes.FilterByPredicate([] (const UGameFlowGraphNode* GraphNode)
	{
		return GraphNode->IsActiveNode();
	});
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

void UGameFlowGraph::OnValidateGraph()
{
	const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	GraphSchema->ValidateAsset(*this);
}

void UGameFlowGraph::OnDebugModeUpdated(bool bEnabled)
{
	const TArray<UGameFlowGraphNode*> GameFlowNodes = reinterpret_cast<const TArray<UGameFlowGraphNode*>&>(Nodes);
	for(UGameFlowGraphNode* Node : GameFlowNodes)
	{
		Node->SetDebugEnabled(bEnabled);
	}
}

#if WITH_HOT_RELOAD

void UGameFlowGraph::OnHotReload(EReloadCompleteReason ReloadCompleteReason)
{
	const TArray<UGameFlowGraphNode*> ReloadedNodes = reinterpret_cast<const TArray<UGameFlowGraphNode*>&>(Nodes);
	for(UGameFlowGraphNode* Node : ReloadedNodes)
	{
		if(Node != nullptr)
		{
			Node->OnLiveOrHotReloadCompile();
		}
	}
}

#endif

#if WITH_LIVE_CODING && (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 4) || ENGINE_MAJOR_VERSION > 5

void UGameFlowGraph::OnLiveCompile(FName Name, ECompiledInUObjectsRegisteredStatus Status)
{
	const TArray<UGameFlowGraphNode*> ReloadedNodes = reinterpret_cast<const TArray<UGameFlowGraphNode*>&>(Nodes);
	for(UGameFlowGraphNode* Node : ReloadedNodes)
	{
		Node->OnLiveOrHotReloadCompile();
	}
}

#else if WITH_LIVE_CODING && (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 4) || ENGINE_MAJOR_VERSION < 5

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
	// We want to use a set of UGameFlowGraphNode type.
	TSet<const UGameFlowGraphNode*> ModifiedNodes;
	for (const UEdGraphNode* Node : Action.Nodes)
	{
		ModifiedNodes.Add(CastChecked<const UGameFlowGraphNode>(Node));
	}

	// Handle the case where added nodes are selected upon creation.
	if(Action.Action == (EEdGraphActionType)(GRAPHACTION_AddNode | GRAPHACTION_SelectNode))
	{
		Super::NotifyGraphChanged(Action);
		OnNodesAdded(ModifiedNodes);
		OnGraphNodesSelected.ExecuteIfBound(ModifiedNodes);
	}
	
	// We rebuild the graph only on Add and remove node actions, selection node
	// will keep it as it is to avoid bGraphDataInvalid error.
	switch(Action.Action)
	{
		case GRAPHACTION_SelectNode:
			{
				OnGraphNodesSelected.ExecuteIfBound(ModifiedNodes);
				break;
			}
	    case GRAPHACTION_AddNode:
			{
				Super::NotifyGraphChanged(Action);
				OnNodesAdded(ModifiedNodes);
				break;
			}
	    case GRAPHACTION_RemoveNode:
			{
				Super::NotifyGraphChanged(Action);
				OnNodesRemoved(ModifiedNodes);
				break;
			}

	    case GRAPHACTION_Default:
			{
				break;
			}
		
		default: break;
	}
}

void UGameFlowGraph::OnReplaceGraphNode()
{
	const TSharedRef<SGameFlowReplaceNodeDialog> ReplaceNodeDialog = SNew(SGameFlowReplaceNodeDialog);
	const int32 PressedButtonIndex = ReplaceNodeDialog->ShowModal();
	UClass* PickedClass = ReplaceNodeDialog->GetPickedClass();
	
	// Has the user picked a valid replacement class and clicked the "Replace" button?
	if(PressedButtonIndex == 0 && PickedClass != nullptr
		&& ReplaceNodeDialog->ShouldReplaceAll())
	{
		const TArray<UGameFlowGraphNode*> GraphNodes = GetNodesOfClass(PickedClass);
		FGameFlowSchemaAction_ReplaceNode ReplaceNodeAction(nullptr, PickedClass);
		// Replace multiple nodes in a single action and transaction.
		ReplaceNodeAction.PerformAction_ReplaceAll(GraphNodes, this);
	}
}

void UGameFlowGraph::OnBreakpointHit(UGameFlowGraphNode* GraphNode, UEdGraphPin* GraphPin)
{
	OnBreakpointHitRequest.Execute(GraphNode, GraphPin);
}

void UGameFlowGraph::OnNodesRemoved(const TSet<const UGameFlowGraphNode*> RemovedNodes)
{
	for(const UGameFlowGraphNode* GraphNode : RemovedNodes)
	{
		// Remove node from the game flow asset.
		UGameFlowNode* NodeAsset = GraphNode->GetNodeAsset();
		GameFlowAsset->RemoveNode(NodeAsset);
	}
}

void UGameFlowGraph::OnNodesAdded(const TSet<const UGameFlowGraphNode*> AddedNodes)
{
	for(const UGameFlowGraphNode* GraphNode : AddedNodes)
	{
		UGameFlowNode* Node = GraphNode->GetNodeAsset();
		// Input and output nodes are also stored in separates maps, so we need to register them.
		if(Node->IsA(UGameFlowNode_Input::StaticClass()))
		{
			GameFlowAsset->CustomInputs.Add(Node->GetFName(), CastChecked<UGameFlowNode_Input>(Node));
		}
		else if(Node->IsA(UGameFlowNode_Output::StaticClass()))
		{
			GameFlowAsset->CustomOutputs.Add(Node->GetFName(), CastChecked<UGameFlowNode_Output>(Node));
		}
		// Register observed node inside game flow asset.
		GameFlowAsset->AddNode(Node);
	}
}

void UGameFlowGraph::RebuildGraphFromAsset()
{
	const UGameFlowGraphSchema* GameFlowSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	
	// Recreate all game flow asset registered nodes, including orphan nodes.
	// Orphans are nodes which does not share connections with any parent node,
	// e.g. their input pins have no links.
	for(UGameFlowNode* NodeAsset : GameFlowAsset->GetNodes())
	{
		UGameFlowGraphNode* GraphNode = FGameFlowNodeSchemaAction_CreateOrDestroyNode::RecreateNode(NodeAsset, this);
		GraphNode->NodePosX = NodeAsset->GraphPosition.X;
		GraphNode->NodePosY = NodeAsset->GraphPosition.Y;
	}
	 
	// Recreate all graph node connections.
	GameFlowSchema->RecreateGraphNodesConnections(*this);
}





