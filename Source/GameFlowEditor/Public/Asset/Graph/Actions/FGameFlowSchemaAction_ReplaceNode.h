#pragma once

#include "CoreMinimal.h"
#include "../GameFlowGraph.h"
#include "EdGraph/EdGraphSchema.h"
#include "Nodes/GameFlowNode.h"

struct GAMEFLOWEDITOR_API FGameFlowSchemaAction_ReplaceNode : public FEdGraphSchemaAction
{
	
private:
	UGameFlowGraphNode* NodeToReplace;
    TSubclassOf<UGameFlowNode> NodeReplacementClass;
	
public:

	FGameFlowSchemaAction_ReplaceNode()
	{
		this->NodeToReplace = nullptr;
		this->NodeReplacementClass = nullptr;
	}
	
	FGameFlowSchemaAction_ReplaceNode(UGameFlowGraphNode* NodeToReplace, TSubclassOf<UGameFlowNode> NodeReplacementClass)
	{
		this->NodeToReplace = NodeToReplace;
		this->NodeReplacementClass = NodeReplacementClass;
	}
	
	FGameFlowSchemaAction_ReplaceNode(const FGameFlowSchemaAction_ReplaceNode& Other)
		: FEdGraphSchemaAction(Other.GetCategory(), Other.GetMenuDescription(), Other.GetTooltipDescription(), Other.GetGrouping()),
		  NodeToReplace(Other.NodeToReplace), NodeReplacementClass(Other.NodeReplacementClass)
	{
	}
	
	FGameFlowSchemaAction_ReplaceNode(UGameFlowGraphNode* NodeToReplace, TSubclassOf<UGameFlowNode> NodeReplacementClass,
		const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
		, NodeToReplace(NodeToReplace), NodeReplacementClass(NodeReplacementClass)
	{
	}
	
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override;
	virtual TArray<UGameFlowGraphNode*> PerformAction_ReplaceAll(TArray<UGameFlowGraphNode*> NodesToReplace, UGameFlowGraph* Graph);
	virtual UGameFlowGraphNode* ReplaceNode(UGameFlowGraphNode* Node_ToReplace, UClass* ReplacementClass);
};
