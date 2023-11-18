// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowAsset.h"
#include "UObject/Object.h"
#include "GameFlowGraph.generated.h"

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
	
	UGameFlowGraph();

	/** Initialize the game flow graph. */
	virtual void InitGraph();
	
	/** Compile the current graph data to an asset.
	 * @param Asset The asset compiled by the graph.
	 * @return True if asset was compiled successfully, false
	 *         otherwise.
	 */
	virtual bool CompileGraph(UObject* Asset);
	
};
