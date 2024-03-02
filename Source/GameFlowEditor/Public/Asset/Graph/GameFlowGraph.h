// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowAsset.h"
#include "Nodes/GameFlowGraphNode.h"
#include "Utils/GameFlowEditorSubsystem.h"
#include "GameFlowGraph.generated.h"

class GameFlowAssetToolkit;

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
	/* Asset currently edited by this graph. */
	UPROPERTY()
	TObjectPtr<UGameFlowAsset> GameFlowAsset;

private:
	TObjectPtr<GameFlowAssetToolkit> GameFlowEditor;
	
public:
	UGameFlowGraph();
	
	void InitGraph();
	virtual void SubscribeToEditorCallbacks(GameFlowAssetToolkit* Editor);
    
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
	
	void OnGraphCompile();
	void OnSaveGraph();
	void OnDummyReplacementRequest();
    
#if WITH_HOT_RELOAD
	void OnHotReload(EReloadCompleteReason ReloadCompleteReason);
#endif
#if WITH_LIVE_CODING
	void OnLiveCompile(FName Name);
#endif
	void RebuildGraphFromAsset();
	virtual void NotifyGraphChanged(const FEdGraphEditAction& Action) override;
	
	void ReplaceDummyNode(UGameFlowGraphNode* DummyNode, UClass* ReplacementClass) const;
	
protected:
	virtual void OnNodesAdded(const TSet<UGameFlowGraphNode*> AddedNodes);
	virtual void OnNodesRemoved(const TSet<UGameFlowGraphNode*> RemovedNodes);
	virtual void OnNodesSelected(const TSet<UGameFlowGraphNode*> SelectedNodes);
};







