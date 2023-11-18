// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/GameFlowNode.h"

/**
 * Graph schema action responsible for creating new nodes.
 */
struct GAMEFLOWEDITOR_API FGameFlowNodeSchemaAction_NewNode : public FEdGraphSchemaAction
{
private:
	
	/* The type of the node to be created. */
	TSubclassOf<UGameFlowNode> NodeClass;
	
public:

	FGameFlowNodeSchemaAction_NewNode(const FGameFlowNodeSchemaAction_NewNode& Other)
		: FEdGraphSchemaAction(Other.GetCategory(), Other.GetMenuDescription(), Other.GetTooltipDescription(), Other.GetGrouping()),
		  NodeClass(Other.NodeClass)
	{
	}
	
	FGameFlowNodeSchemaAction_NewNode(TSubclassOf<UGameFlowNode> NodeClass, const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
	    , NodeClass(NodeClass)
	{
	}
	
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override;
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode) override;
};
