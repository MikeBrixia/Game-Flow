
#include "Asset/Graph/Actions/GameFlowNodeSchemaAction_DestroyNode.h"
#include "Asset/Graph/GameFlowGraph.h"

UEdGraphNode* FGameFlowNodeSchemaAction_DestroyNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                                   const FVector2D Location, bool bSelectNewNode)
{
	FScopedTransaction Transaction(NSLOCTEXT("GameFlowEditor", "DestroyNode", "Destroy Node"));
	
	// Record changes on the game flow graph and asset.
	ParentGraph->Modify();
	NodeToDestroy->Modify();
	// Record changes on the game flow graph and asset.
	NodeToDestroy->DestroyNode();
	
	return nullptr;
}

