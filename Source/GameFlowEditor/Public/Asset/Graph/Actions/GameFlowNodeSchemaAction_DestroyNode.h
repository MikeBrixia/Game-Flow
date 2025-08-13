#pragma once

class UGameFlowGraphNode;

struct FGameFlowNodeSchemaAction_DestroyNode : public FEdGraphSchemaAction
{
	UGameFlowGraphNode* NodeToDestroy;
	
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location,
		bool bSelectNewNode = true) override;
};
