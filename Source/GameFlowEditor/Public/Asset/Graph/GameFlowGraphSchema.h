﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowGraph.h"
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
	* Connect a pin to another node default pin.
	* @remark Default pin are the "Exec" or "Out" pins of each node. In case of missing
	*         default pins, first node pin will be considered the default pin.
	* @param FromPin The pin trying to connect to a default pin.
	* @param GraphNode The target graph node for the connection.
	*/
	void ConnectToDefaultPin(UEdGraphPin* FromPin, UEdGraphNode* GraphNode) const;
	
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
};
