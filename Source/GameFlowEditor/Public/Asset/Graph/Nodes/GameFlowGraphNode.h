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
	// Allow game flow factory to access any member of this class.
	// Game Flow Factory should be the only class to have access
	// to initialization methods.
	friend class UGameFlowFactory;
	
	GENERATED_BODY()
	
public:

	UGameFlowGraphNode();
	
	/** Create the default pins for this node. */
	virtual void AllocateDefaultPins() override;
	/** Create the visual widget for this node. */
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;

protected:

	/** Initialize this node properties. */
	virtual void InitNode();
};
