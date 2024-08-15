// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Debug/FGameFlowGraphDebugger.h"
#include "GameFlowAsset.h"
#include "Nodes/GameFlowGraphNode.h"
#include "GameFlowGraph.generated.h"

class GameFlowAssetToolkit;

DECLARE_DELEGATE_OneParam(FOnGraphNodesSelected, TSet<UGameFlowGraphNode*>)

/**
 * Class representing a Game Flow graph.
 * The graph is responsible for managing and containing
 * nodes as well as performing operations on them.
 */
UCLASS(NotBlueprintable, NotBlueprintType)
class GAMEFLOWEDITOR_API UGameFlowGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	/** Asset currently edited by this graph. */
	UPROPERTY()
	TObjectPtr<UGameFlowAsset> GameFlowAsset;

	/** The debugged instance of the current edited GameFlowAsset. */
	UPROPERTY(Transient)
	TObjectPtr<UGameFlowAsset> DebuggedAssetInstance;
	
	/** Called when graph nodes gets selected*/
	FOnGraphNodesSelected OnGraphNodesSelected;

	/** Game Flow graph debugger instance shared by all graphs. */
	TSharedPtr<FGameFlowGraphDebugger> Debugger;
	
	UGameFlowGraph();
	
	void InitGraph();

	/**
	 * Set the instance of the inspected asset you want to debug.
	 * @remark Instance must be a child of the inspected game flow asset.
	 */
	void SetDebuggedInstance(UGameFlowAsset* Instance);
	
	/**
	 * @brief Find all graph nodes with an asset of requested type inside the graph.
	 * @param NodeClass The requested type.
	 * @return an array of graph nodes with node asset of requested type.
	 */
	TArray<UGameFlowGraphNode*> GetNodesOfClass(const TSubclassOf<UGameFlowNode> NodeClass) const;

	/**
	 * @brief Find the graph node associated with the given game flow node asset.
	 * @param NodeAsset The associated node asset.
	 * @return Associated graph node.
	 */
	UGameFlowGraphNode* GetGraphNodeByAsset(const UGameFlowNode* NodeAsset) const;
	
	/**
	 * @brief Find all orphan nodes inside the graph.
	 * @return An array of orphan graph nodes.
	 */
	TArray<UGameFlowGraphNode*> GetOrphanNodes() const;

	/**
	 * Find all roots nodes placed inside the graph.
	 * @return An array of all root nodes, empty if no root nodes could be found.
	 */
	TArray<UGameFlowGraphNode*> GetRootNodes() const;

	/**
	 * Find all the currently active and executed nodes inside the graph.
	 * @return An array of all currently active nodes in the graph, empty if none could be found.
	 */
	TArray<UGameFlowGraphNode*> GetActiveNodes() const;
	
	void OnSaveGraph();
	void OnValidateGraph();
	void OnReplaceGraphNode();

#if WITH_HOT_RELOAD
	void OnHotReload(EReloadCompleteReason ReloadCompleteReason);
#endif
#if WITH_LIVE_CODING
	void OnLiveCompile(FName Name);
#endif
	void RebuildGraphFromAsset();
	virtual void NotifyGraphChanged(const FEdGraphEditAction& Action) override;

protected:
	virtual void OnNodesAdded(const TSet<UGameFlowGraphNode*> AddedNodes);
	virtual void OnNodesRemoved(const TSet<UGameFlowGraphNode*> RemovedNodes);
};







