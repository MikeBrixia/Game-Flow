// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowAsset.h"
#include "GameFlowGraph.h"
#include "Nodes/GameFlowNode_Dummy.h"
#include "Nodes/GameFlowNode_Input.h"
#include "Nodes/GameFlowNode_Output.h"
#include "UObject/Object.h"
#include "GameFlowGraphSchema.generated.h"

UCLASS(NotBlueprintable, NotBlueprintType)
class GAMEFLOWEDITOR_API UGameFlowGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	
	virtual FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID,
	                                                                float InZoomFactor,
	                                                                const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
	                                                                UEdGraph* InGraphObj) const override;
	
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
    virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;
	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const override;
	
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual UEdGraphNode* CreateSubstituteNode(UEdGraphNode* Node, const UEdGraph* Graph, FObjectInstancingGraph* InstanceGraph, TSet<FName>& InOutExtraNames) const override;
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
	 * @return True if branch compilation was successful, false otherwise.
	 */
	bool CompileGraphBranch(UGameFlowGraphNode* RootNode) const;

	/**
	 * @brief Compile a single node inside the graph.
	 * @param GraphNode The node to compile
	 * @param Directions Specify in which directions you want to compile the pins.
	 * @return True if node was compiled successfully, false otherwise.
	 */
	bool CompileGraphNode(UGameFlowGraphNode* GraphNode, const TArray<EEdGraphPinDirection> Directions) const;
	
	/**
	 * @brief Recreate all logical and visual connections between all the graph nodes.
	 * @param Graph The graph in which the operation takes place.
	 */
	void RecreateGraphNodesConnections(const UGameFlowGraph& Graph) const;
	
	/**
	 * @brief Recreate nodes connections starting from a given node.
	 * @param Graph The graph in which the operation takes place.
	 * @param RootNode The root of a Game Flow branch. In this case the root could be any selected node. 
	 */
	void RecreateBranchConnections(const UGameFlowGraph& Graph, UGameFlowGraphNode* RootNode) const;

	/**
	 * @brief Recreate connections for the specified node.
	 * @param Graph The graph in which the operation takes place.
	 * @param GraphNode The node to recreate connections for
	 * @param Directions Specify in which directions you want to recreate connections.
	 */
	void RecreateNodeConnections(const UGameFlowGraph& Graph, UGameFlowGraphNode* GraphNode, const TArray<EEdGraphPinDirection> Directions) const;
	
	/**
	 * @brief Ensure that the supplied asset does not contain any corrupted data or errors..
	 * @param Graph The graph in which the operation takes place.
	 */
	void ValidateAsset(UGameFlowGraph& Graph) const;

	/**
	 * @brief Ensure that the supplied node asset does not contain any corrupted data or errors.
	 * @param GraphNode The Node to validate.
	 */
	void ValidateNodeAsset(UGameFlowGraphNode* GraphNode) const;

	/**
	 * @brief Substitute the given graph node with a replacement dummy node.
	 * @param GraphNode The graph node you want to substitute.
	 * @param DummyNodeClass The class of the dummy node to use as the replacement.
	 */
	void SubstituteWithDummyNode(UGameFlowGraphNode* GraphNode, const TSubclassOf<UGameFlowNode_Dummy> DummyNodeClass) const;

protected:
    
	virtual UGameFlowNode_Input* CreateDefaultInputs(UGameFlowGraph& Graph) const;
	virtual UGameFlowNode_Output* CreateDefaultOutputs(UGameFlowGraph& Graph) const;
};
