// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowAsset.h"
#include "Nodes/GameFlowGraphNode.h"
#include "UObject/Object.h"
#include "Utils/GameFlowEditorSubsystem.h"
#include "GameFlowGraph.generated.h"

class GameFlowAssetToolkit;

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
	
	UGameFlowGraph();
	
	// -------------- GRAPH METADATA --------------------------------------------
	
	/* Asset currently edited by this graph. */
	UPROPERTY()
	TObjectPtr<UGameFlowAsset> GameFlowAsset;

	UPROPERTY()
	TArray<UGameFlowGraphNode*> RootNodes;
	
private:
	TObjectPtr<GameFlowAssetToolkit> GameFlowEditor;

public:
	void InitGraph();
	virtual void SubscribeToEditorCallbacks(GameFlowAssetToolkit* Editor);
	
	/**
	 * Compile Game Flow graph.
	 * @param Asset The asset compiled by the graph.
	 */
	void OnGraphCompile(UGameFlowAsset* Asset);

	/**
	 * @brief Save all changes applied to the graph.
	 */
	void OnSaveGraph();

protected:
	virtual void NotifyGraphChanged(const FEdGraphEditAction& Action) override;
	
private:
	
	/**
	 * @brief Compile graph starting from all inputs
	 * @return True if the entire graph was compiled, false otherwise.
	 */
	bool CompileGraph();
	
	/**
	 * @brief Start compiling graph from a specified input node
	 * @param InputNode Input which represents a root of the network.
	 */
	bool CompileGraphFromInputNode(UGameFlowGraphNode* InputNode);
    
	/**
	 * @brief Rebuild graph using GameFlow asset data.
	 */
	void RebuildGraphFromAsset();
	
};


