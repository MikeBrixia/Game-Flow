// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowAsset.h"
#include "Nodes/GameFlowGraphNode.h"
#include "UObject/Object.h"
#include "GameFlowGraph.generated.h"

/**
 * Class representing a Game Flow graph.
 * The graph is responsible for managing and containing
 * nodes as well as performing operations on them.
 */
UCLASS()
class GAMEFLOWEDITOR_API UGameFlowGraph : public UEdGraph
{
	GENERATED_BODY()

public:

	/* Asset currently edited by this graph. */
	UPROPERTY()
	TObjectPtr<UGameFlowAsset> GameFlowAsset;

	UPROPERTY()
	TArray<UGameFlowGraphNode*> RootNodes;
	
	UGameFlowGraph();

	/** Initialize the game flow graph. */
	virtual void InitGraph();
	
	/** Compile the current graph data to an asset.
	 * @param Asset The asset compiled by the graph.
	 * @return True if asset was compiled successfully, false
	 *         otherwise.
	 */
	void CompileGraph(UGameFlowAsset* Asset);

	/**
	 * @brief Starting from a input node, compile all it's connections onto a tree.
	 * @param InputNode The node from which the compilation will start.
	 */
	void CompileInputNode(UGameFlowGraphNode* InputNode);

	/**
	 * @brief Rebuild graph using GameFlow asset data.
	 */
	void RebuildGraphFromAsset();
	
	virtual void NotifyGraphChanged() override;
};

