// Fill out your copyright notice in the Description page of Project Settings.

#include "Utils/GameFlowFactory.h"
#include "GameFlowAsset.h"
#include "Kismet2/BlueprintEditorUtils.h"

UGameFlowFactory::UGameFlowFactory()
{
	SupportedClass = UGameFlowAsset::StaticClass();
	bCreateNew = true;
	bEditAfterNew = false;
}

UObject* UGameFlowFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
										   UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UGameFlowAsset>(InParent, InClass, InName, Flags, Context);
}

UEdGraph* UGameFlowFactory::CreateGraph(UObject* Asset, const TSubclassOf<UGameFlowGraph> GraphClass, const TSubclassOf<UEdGraphSchema> SchemaClass)
{
	checkf(Asset, TEXT("Asset cannot be nullptr when creating graph"));
	checkf(GraphClass, TEXT("Graph class cannot be null when creating graph"));
	checkf(SchemaClass, TEXT("Schema class cannot be null when creating graph"));
	
	// Create and initialize the Game Flow graph.
	UGameFlowGraph* Graph = Cast<UGameFlowGraph>(FBlueprintEditorUtils::CreateNewGraph(Asset, "Flow Graph", GraphClass, SchemaClass));
	Graph->GameFlowAsset = CastChecked<UGameFlowAsset>(Asset);
	Graph->SetFlags(RF_Transactional);
	Graph->InitGraph();
	
	return Graph;
}

template <typename TGraphType, typename TGraphSchemaType>
TGraphType* UGameFlowFactory::CreateGraph(UObject* Asset)
{
	UClass* GraphClass = TGraphType::StaticClass();
	UClass* SchemaClass = TGraphSchemaType::StaticClass();

	// Make sure we're trying to create a valid graph.
	checkf(Asset, TEXT("Asset cannot be nullptr when creating graph"));
	checkf(GraphClass != nullptr && GraphClass->IsChildOf(UEdGraph::StaticClass()), TEXT("Invalid graph class! Your graph class should derive from UEdGraph"));
	checkf(SchemaClass != nullptr && SchemaClass->IsChildOf(UEdGraphSchema::StaticClass()), TEXT("Invalid schema class! Your schema class should derive from UEdGraphSchema"));

	// Create and initialize the Game Flow graph.
	UGameFlowGraph* Graph = Cast<UGameFlowGraph>(FBlueprintEditorUtils::CreateNewGraph(Asset, "Flow Graph", GraphClass, SchemaClass));
	Graph->GameFlowAsset = CastChecked<UGameFlowAsset>(Asset);
	Graph->SetFlags(RF_Transactional);
	Graph->InitGraph();
	
	return Graph;
}


