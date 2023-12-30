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
	/* Asset currently edited by this graph. */
	UPROPERTY()
	TObjectPtr<UGameFlowAsset> GameFlowAsset;

	UPROPERTY()
	TArray<UGameFlowGraphNode*> RootNodes;

	UPROPERTY()
	TMap<uint32, UGameFlowGraphNode*> GraphNodes;
	
private:
	TObjectPtr<GameFlowAssetToolkit> GameFlowEditor;
	
public:
	UGameFlowGraph();
	
	void InitGraph();
	virtual void SubscribeToEditorCallbacks(GameFlowAssetToolkit* Editor);
	
	void OnGraphCompile();
	void OnSaveGraph();

	void RebuildGraphFromAsset();

	virtual void NotifyGraphChanged(const FEdGraphEditAction& Action) override;
};


