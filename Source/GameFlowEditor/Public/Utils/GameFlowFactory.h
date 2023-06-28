// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Asset/Graph/GameFlowGraph.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "GameFlowFactory.generated.h"

/**
 * Factory class useful to create Game Flow assets, graphs and nodes.
 */
UCLASS()
class GAMEFLOWEDITOR_API UGameFlowFactory final : public UFactory
{
	GENERATED_BODY()

public:
	
	UGameFlowFactory();

	/** Create a new game flow asset object. */
	UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	/** Create a brand new Game Flow graph.
	 * @param Asset The asset the graph will be responsible to edit.
	 * @param GraphClass The class type of the graph to create.
	 * @param SchemaClass The class type of the schema the
	 *                    newly created graph is going to obey.
	 * @return A pointer to the created graph.
	 */
	static UEdGraph* CreateGraph(UObject* Asset, const TSubclassOf<UGameFlowGraph> GraphClass, const TSubclassOf<UEdGraphSchema> SchemaClass);

	/** Create a brand new Game Flow graph(generics version).
	 * @tparam TGraphType The class type of the graph to create.
	 * @tparam TGraphSchemaType The class type of the schema the
	 *                          newly created graph is going to obey
	 * @param Asset The asset the graph will be responsible to edit.
	 * @return A pointer to the created graph.
	 */
	template<typename TGraphType, typename TGraphSchemaType>
	static TGraphType* CreateGraph(UObject* Asset);

	/** Create a brand new Game Flow node
	 * @param NodeClass The class of the node to create.
	 * @param Graph The graph to which the node is going to belong.
	 * @return A pointer to the created graph node.
	 */
	static UEdGraphNode* CreateNode(TSubclassOf<UGameFlowGraphNode> NodeClass, UEdGraph* Graph);

	/** Create a brand new Game Flow node
	 * @tparam TNodeClass The class of the node to create.
	 * @param Graph The graph to which the node is going to belong.
	 * @return A pointer to the created graph node.
	 */
	template<typename TNodeClass>
	static TNodeClass* CreateNode(UEdGraph* Graph);
};


