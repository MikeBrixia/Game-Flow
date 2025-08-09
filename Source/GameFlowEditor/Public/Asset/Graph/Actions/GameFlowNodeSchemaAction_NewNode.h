// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GameFlowGraph.h"
#include "EdGraph/EdGraphSchema.h"
#include "Nodes/GameFlowNode.h"

/**
 * Graph schema action responsible for creating and destroying new game flow nodes.
 */
struct GAMEFLOWEDITOR_API FGameFlowNodeSchemaAction_CreateOrDestroyNode : public FEdGraphSchemaAction
{

private:
	
	/* The type of the node to be created. */
	TSubclassOf<UGameFlowNode> NodeClass;
	
public:

	FGameFlowNodeSchemaAction_CreateOrDestroyNode()
	{
	}
	
	FGameFlowNodeSchemaAction_CreateOrDestroyNode(const FGameFlowNodeSchemaAction_CreateOrDestroyNode& Other)
		: FEdGraphSchemaAction(Other.GetCategory(), Other.GetMenuDescription(), Other.GetTooltipDescription(), Other.GetGrouping()),
		  NodeClass(Other.NodeClass)
	{
	}
	
	FGameFlowNodeSchemaAction_CreateOrDestroyNode(TSubclassOf<UGameFlowNode> NodeClass, const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
	    , NodeClass(NodeClass)
	{
	}
	
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override;
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode) override;

	/**
	 * Destroy a game flow node and record a transaction for it.
	 * @param GraphNode The node to destroy
	 * @return true if node was destroyed and transaction recorded successfully, false otherwise.
	 */
    virtual void PerformAction_DestroyNode(UGameFlowGraphNode* GraphNode);
	
	/**
	 * Create a brand new game flow node.
	 * @param NodeClass the class of encapsulated node asset, used to create it.
	 * @param GameFlowGraph the graph in which the operation takes place.
	 * @param FromPin valid if this node has been created by dragging another node pin, nullptr otherwise; Can be nullptr.
	 * @return A brand new and ready-to-use game flow graph node.
	 */
	static UGameFlowGraphNode* CreateNode(UClass* NodeClass, UGameFlowGraph* GameFlowGraph, FName NodeName =EName::None, UEdGraphPin* FromPin = nullptr);

	/**
	 * Create a brand new game flow node.
	 * @param NodeAsset The node asset from which the graph node will be created.
	 * @param GameFlowGraph the graph in which the operation takes place.
	 * @param FromPin valid if this node has been created by dragging another node pin, nullptr otherwise; Can be nullptr.
	 * @return A brand new and ready-to-use game flow graph node.
	 */
	static UGameFlowGraphNode* CreateNode(UGameFlowNode* NodeAsset, UGameFlowGraph* GameFlowGraph, FName NodeName =EName::None, UEdGraphPin* FromPin = nullptr);
};
