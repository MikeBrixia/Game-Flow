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

	/**
	 * @brief Is the connection between the two pins allowed?
	 * @return True if connection was allowed, false otherwise.
	 */
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;

	/**
	 * @brief Connect pin A to pin B. Game Flow schema override supports
	 *        live compilation features for game flow assets.
	 * @return True if connection was successful, false otherwise.
	 */
	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;

	/**
	* Connect a pin to another node default pin.
	* @remark Default pin are the "Exec" or "Out" pins of each node. In case of missing
	*         default pins, first node pin will be considered the default pin.
	* @param FromPin The pin trying to connect to a default pin.
	* @param GraphNode The target graph node for the connection.
	* @param Graph The graph in which the operation takes place.
	*/
	void ConnectToDefaultPin(UEdGraphPin* FromPin, UEdGraphNode* GraphNode, const UGameFlowGraph* Graph) const;
	
	/**
	* @brief Break a single connection between two pins. Game Flow schema
	*        override supports live compilation features for game flow assets.
	 */
	virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;

	/**
	* @brief Break all connections of a target pin. Game Flow schema
	*        override supports live compilation features for game flow assets.
	 * @param TargetPin The target pin that will break all it's connections
	 */
	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const override;

	/**
	 * @brief Populate target graph with Game Flow default nodes.
	 * @param Graph The graph in which the operation takes place.
	 */
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;

	/**
	 * @brief Get/Create the game flow graph contextual menu with all available
	 *        nodes displayed, ordered and ready to be created.
	 * @param ContextMenuBuilder The object used to expand/build the menu.
	 */
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
	 * Can we create a graph node for the given game flow node class?
	 * @param Class the class of the game flow node.
	 * @return True if we can create graph node from the game flow node class,
	 *         false otherwise.
	 */
	bool CanCreateGraphNodeForClass(UClass* Class) const;
	
	/**
	 * @brief Substitute the given graph node with a replacement dummy node.
	 * @param GraphNode The graph node you want to substitute.
	 * @param DummyNodeClass The class of the dummy node to use as the replacement.
	 */
	void SubstituteWithDummyNode(UGameFlowGraphNode* GraphNode, const TSubclassOf<UGameFlowNode_Dummy> DummyNodeClass) const;

	/**
	 * @brief Substitute a graph node with a substitute node. Substitute node will encapsulate
	 *        target node asset(UGameFlowNode) passed with InstanceGraph.
	 * @param Node The node you want to substitute.
	 * @param Graph The graph in which the operation takes place. Can be nullptr, it's here
	 *              only because of the override.
	 * @param InstanceGraph The mapping between the node asset substitute and the node asset
	 *                      to replace. To be clear, inside this structure you need to pass
	 *                      the Node assets(UGameFlowNode)
	 * @param InOutExtraNames You can pass an empty set, it's here only because of the override.
	 * @return The substituted graph node.
	 */
	virtual UEdGraphNode* CreateSubstituteNode(UEdGraphNode* Node, const UEdGraph* Graph, FObjectInstancingGraph* InstanceGraph, TSet<FName>& InOutExtraNames) const override;
	
protected:
	
	/**
	 * Ensure that the graph node and it's contained node asset data are not different.
	 * @remarks This is mainly used to avoid situations where the graph node pins and connections
	 *          do not match the node asset ones.
	 * @param GraphNode The graph node to check and align.
	 */
	void AlignNodeAssetToGraphNode(UGameFlowGraphNode* GraphNode) const;
	
	virtual UGameFlowNode_Input* CreateDefaultInputs(UGameFlowGraph& Graph) const;
	virtual UGameFlowNode_Output* CreateDefaultOutputs(UGameFlowGraph& Graph) const;
};
