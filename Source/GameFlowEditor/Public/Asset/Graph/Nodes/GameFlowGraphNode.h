// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameFlowGraphNode.generated.h"

/**
 * A node used inside the Game Flow graph.
 */
UCLASS()
class GAMEFLOWEDITOR_API UGameFlowGraphNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
};
