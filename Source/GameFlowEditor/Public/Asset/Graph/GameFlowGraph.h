// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameFlowGraph.generated.h"

/**
 * Main class of the GameFlow editor graph.
 */
UCLASS()
class GAMEFLOWEDITOR_API UGameFlowGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	
	UGameFlowGraph();

	/** Initialize the game flow graph. */
	virtual void InitGraph();
};
