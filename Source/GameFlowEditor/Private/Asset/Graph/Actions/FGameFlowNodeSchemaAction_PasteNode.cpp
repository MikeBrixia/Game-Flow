#include "Asset/Graph/Actions/FGameFlowNodeSchemaAction_PasteNode.h"

UEdGraphNode* FGameFlowNodeSchemaAction_PasteNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FVector2D Location, bool bSelectNewNode)
{
	return FEdGraphSchemaAction::PerformAction(ParentGraph, FromPin, Location, bSelectNewNode);
}

UEdGraphNode* FGameFlowNodeSchemaAction_PasteNode::PerformAction(UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins,
	const FVector2D Location, bool bSelectNewNode)
{
	return FEdGraphSchemaAction::PerformAction(ParentGraph, FromPins, Location, bSelectNewNode);
}
