// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowEditor.h"
#include "GameFlowAsset.h"
#include "Asset/Graph/GameFlowGraph.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Nodes/GameFlowNode.h"
#include "UObject/Object.h"
#include "UGameFlowNodeFactory.generated.h"

/**
 * Factory used to create Game Flow nodes.
 */
UCLASS()
class GAMEFLOWEDITOR_API UGameFlowNodeFactory final : public UFactory
{
	GENERATED_BODY()
	
public:

	/* Create a brand new Game Flow Node Asset. */
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
		                             UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	
	/**
	 * @brief Create a graph node with 'NodeAsset' encapsulated inside it.
	 * @param NodeAsset The GameFlow Node which will be encapsulated inside the graph node
	 * @param Graph The Graph inside which the graph node will be placed.
	 * @return A brand new graph node.
	 */
	static UGameFlowGraphNode* CreateGraphNode(UGameFlowNode* NodeAsset, UGameFlowGraph* Graph);

	/**
	 * @brief 
	 * @param NodeClass The class of the node which will be encapsulated inside the graph node
	 * @param ParentAsset The GameFlow asset who owns the encapsulated node.
	 * @param Graph The graph who owns the new graph node.
	 * @return The created graph node.
	 */
	static UGameFlowGraphNode* CreateGraphNode(const TSubclassOf<UGameFlowNode> NodeClass, UGameFlowAsset* ParentAsset, UGameFlowGraph* Graph);

	/**
	 * @brief Create a brand new game flow node and register it inside parent GameFlowAsset.
	 * @param NodeClass The type of the node to create
	 * @param GameFlowAsset The parent asset in which the brand new node will be registered and stored.
	 * @return The new and initialized game flow node.
	 */
	static UGameFlowNode* CreateGameFlowNode(const TSubclassOf<UGameFlowNode> NodeClass, UGameFlowAsset* GameFlowAsset);
	
};
