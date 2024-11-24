#pragma once
#include "Nodes/GameFlowNode.h"

struct FGameFlowNodeSchemaAction_PasteNode : public FEdGraphSchemaAction
{
	TObjectPtr<UGameFlowGraphNode> NodeToPaste;
	
	FGameFlowNodeSchemaAction_PasteNode()
	{
	}
	
	FGameFlowNodeSchemaAction_PasteNode(const FGameFlowNodeSchemaAction_PasteNode& Other)
		: FEdGraphSchemaAction(Other.GetCategory(), Other.GetMenuDescription(), Other.GetTooltipDescription(), Other.GetGrouping())
	{
	}
	
	FGameFlowNodeSchemaAction_PasteNode(TSubclassOf<UGameFlowNode> NodeClass, const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
	{
	}
	
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override;
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode) override;
};
