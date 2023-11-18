// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowGraphNode.generated.h"

/**
 * A node used inside Game Flow graphs.
 */
UCLASS(NotBlueprintable, NotBlueprintType)
class GAMEFLOWEDITOR_API UGameFlowGraphNode : public UEdGraphNode
{
	friend class UGameFlowNodeFactory;
	
	GENERATED_BODY()

private:

	/* The game flow node asset encapsulated inside this graph node.*/
	TObjectPtr<UGameFlowNode> NodeAsset;
	
public:

	UGameFlowGraphNode();
	
	/** Create the default pins for this node. */
	virtual void AllocateDefaultPins() override;
	
	/** Create the visual widget for this node. */
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	
	/** Get the asset contained inside this graph node. */
	FORCEINLINE UGameFlowNode* GetNodeAsset() const
	{
		return NodeAsset;
	}

protected:
	
	/** Initialize this node properties. */
	virtual void InitNode();
};
