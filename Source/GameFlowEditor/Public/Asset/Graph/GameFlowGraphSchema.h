// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowAsset.h"
#include "GameFlowGraph.h"
#include "Nodes/GameFlowNode_Input.h"
#include "Nodes/GameFlowNode_Output.h"
#include "UObject/Object.h"
#include "GameFlowGraphSchema.generated.h"

/**
 * Schema used by the GameFlow graph.
 */
UCLASS()
class GAMEFLOWEDITOR_API UGameFlowGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	
	virtual FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID,
	                                                                float InZoomFactor,
	                                                                const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
	                                                                UEdGraph* InGraphObj) const override;
	
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
    
	/**
	 * @brief Compile the asset edited by the given graph.
	 * @param Graph The graph to compile
	 * @param GameFlowAsset The asset being edited by the graph editor.
	 * @return True if compilation was successful, false otherwise.
	 */
	bool CompileGraph(const UGameFlowGraph& Graph, UGameFlowAsset* GameFlowAsset) const;

	/**
	 * @brief Compile a single branch inside the Game Flow graph. A branch must be compiled from
	 *        a root node, which is also known as a Game Flow input.
	 * @param RootNode The root of a Game Flow branch. Root nodes are also known as input nodes. 
	 * @param GameFlowAsset The game flow asset to compile.
	 * @return True if branch compilation was successful, false otherwise.
	 */
	bool CompileGraphBranch(UGameFlowGraphNode* RootNode, UGameFlowAsset* GameFlowAsset) const;

	/**
	 * @brief Recreate all logical and visual connections between all the graph nodes.
	 * @param Graph The graph in which the operation takes place.
	 * @param GameFlowAsset The asset from which will be reading the necessary data to rebuild the graph.
	 */
	void RecreateGraphNodesConnections(const UGameFlowGraph& Graph, UGameFlowAsset* GameFlowAsset) const;
	
	/**
	 * @brief Recreate nodes connections starting from a given node.
	 * @param Graph The graph in which the operation takes place.
	 * @param RootNodeAsset The root of a Game Flow branch. In this case the root could be any selected node. 
	 */
	void RecreateBranchConnections(const UGameFlowGraph& Graph, const UGameFlowNode* RootNodeAsset) const;

	/**
	 * @brief Find all nodes inside the graph which do not have any connected input pin.
	 * @param Graph The graph in which the operation takes place.
	 * @return An array of all the graph orphan nodes.
	 */
	TArray<UGameFlowGraphNode*> GetGraphOrphanNodes(const UGameFlowGraph& Graph) const;
	
private:
    
	virtual UGameFlowNode_Input* CreateDefaultInputs(UGameFlowGraph& Graph) const;
	virtual UGameFlowNode_Output* CreateDefaultOutputs(UGameFlowGraph& Graph) const;
};
