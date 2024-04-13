#include "Asset/Graph/Actions/FGameFlowSchemaAction_ReplaceNode.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Asset/Graph/Actions/GameFlowNodeSchemaAction_NewNode.h"

UEdGraphNode* FGameFlowSchemaAction_ReplaceNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
                                                               const FVector2D Location, bool bSelectNewNode)
{
	FScopedTransaction Transaction(NSLOCTEXT("GameFlowEditor", "ReplaceNode", "Replace node"));

	UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(ParentGraph);
	GameFlowGraph->Modify();
	GameFlowGraph->GameFlowAsset->Modify();
	NodeToReplace->Modify();
	NodeToReplace->GetNodeAsset()->Modify();
	
	return ReplaceNode(NodeToReplace, NodeReplacementClass);
}

TArray<UGameFlowGraphNode*> FGameFlowSchemaAction_ReplaceNode::PerformAction_ReplaceAll(
	TArray<UGameFlowGraphNode*> NodesToReplace, UGameFlowGraph* Graph)
{
	FScopedTransaction Transaction(NSLOCTEXT("GameFlowEditor", "ReplaceNode", "Replace multiple nodes"));
	TArray<UGameFlowGraphNode*> ReplacementNodes;
	
	Graph->Modify();
	Graph->GameFlowAsset->Modify();

	for(UGameFlowGraphNode* Node : NodesToReplace)
	{
		UGameFlowGraphNode* ReplacementNode = ReplaceNode(Node, NodeReplacementClass);
		ReplacementNodes.Add(ReplacementNode);
	}
	return ReplacementNodes;
}

UGameFlowGraphNode* FGameFlowSchemaAction_ReplaceNode::ReplaceNode(UGameFlowGraphNode* Node_ToReplace, UClass* ReplacementClass)
{
	UGameFlowGraphNode* ReplacementNode = Node_ToReplace;
	if(Node_ToReplace->CanBeReplaced())
	{
		UGameFlowNode* NodeAsset = Node_ToReplace->GetNodeAsset();
		UGameFlowAsset* GameFlowAsset = NodeAsset->GetTypedOuter<UGameFlowAsset>();
		const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(Node_ToReplace->GetSchema());
		
		UGameFlowNode* SubstituteNodeAsset = NewObject<UGameFlowNode>(GameFlowAsset, ReplacementClass, NAME_None,
																	  RF_Transactional);
		FObjectInstancingGraph ObjectInstancingGraph;
		ObjectInstancingGraph.AddNewObject(SubstituteNodeAsset, NodeAsset);
		TSet<FName> InOutExtraNames;
		// Here we can use the copy of the node to create a substitute node.
		ReplacementNode = CastChecked<UGameFlowGraphNode>(
			GraphSchema->CreateSubstituteNode(NodeToReplace, Node_ToReplace->GetGraph(), &ObjectInstancingGraph, InOutExtraNames));
		
		FGameFlowNodeSchemaAction_CreateOrDestroyNode DestroyNodeAction;
		DestroyNodeAction.PerformAction_DestroyNode(Node_ToReplace);
	}
	else
	{
		UE_LOG(LogGameFlow, Error, TEXT("'%s' cannot be replaced! try overriding it's CanBeReplaced() function"),
			*Node_ToReplace->GetName())
	}

	return ReplacementNode;
}
