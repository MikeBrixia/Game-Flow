// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Asset/Graph/GameFlowGraph.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
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
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

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
	template<
		     typename TGraphType = UGameFlowGraph,
	         typename TGraphSchemaType = UGameFlowGraphSchema,
	         typename = TEnableIf<TIsDerivedFrom<TGraphType, UGameFlowGraph>::Value>::Type,
	         typename = TEnableIf<TIsDerivedFrom<TGraphSchemaType, UGameFlowGraphSchema>::Value>::Type
	        >
	static TGraphType* CreateGraph(UObject* Asset)
	{
		UClass* GraphClass = TGraphType::StaticClass();
		UClass* SchemaClass = TGraphSchemaType::StaticClass();
		
		ensureMsgf(Asset, TEXT("Ensure condition failed: Asset cannot be nullptr when creating graph"));
		if (!Asset->IsValidLowLevel()) return nullptr;

		// Create and initialize the Game Flow graph.
		UGameFlowGraph* Graph = Cast<UGameFlowGraph>(FBlueprintEditorUtils::CreateNewGraph(Asset, "Flow Graph", GraphClass, SchemaClass));
		Graph->GameFlowAsset = CastChecked<UGameFlowAsset>(Asset);
		Graph->SetFlags(RF_Transactional);
		Graph->InitGraph();
	
		return Graph;
	}
};


