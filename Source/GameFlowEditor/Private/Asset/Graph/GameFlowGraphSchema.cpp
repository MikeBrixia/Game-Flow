// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowGraphSchema.h"

#include "GameFlowEditor.h"
#include "Asset/Graph/GameFlowConnectionDrawingPolicy.h"
#include "Asset/Graph/GameFlowNodeSchemaAction_NewNode.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Utils/UGameFlowNodeFactory.h"


FConnectionDrawingPolicy* UGameFlowGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID,
                                                                              float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
                                                                              UEdGraph* InGraphObj) const
{
	return new FGameFlowConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements);
}

const FPinConnectionResponse UGameFlowGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	FPinConnectionResponse ConnectionResponse;
	// Ensure that a the user is connecting the output pin A to an input pin B.
	// all other cases should be negated.
	if(A->Direction == EGPD_Output && B->Direction == EGPD_Input)
	{
		ConnectionResponse.Response = CONNECT_RESPONSE_MAKE;
		ConnectionResponse.Message = INVTEXT("Node connection allowed");
	}
	else
	{
		ConnectionResponse.Response = CONNECT_RESPONSE_DISALLOW;
		ConnectionResponse.Message = INVTEXT("Node connection is not allowed");
	}
	return ConnectionResponse;
}

void UGameFlowGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(&Graph);
	if(GameFlowGraph != nullptr)
	{
		UGameFlowAsset* GameFlowAsset = GameFlowGraph->GameFlowAsset;
		
		// Create standard input node.
		UGameFlowNode_Input* StandardInputNode = GameFlowAsset->CreateDefaultStartNode();
		UGameFlowGraphNode* InputGraphNode = UGameFlowNodeFactory::CreateGraphNode(StandardInputNode, GameFlowGraph);
        GameFlowGraph->RootNodes.Add(InputGraphNode);
		
		// Create standard output node.
		UGameFlowNode_Output* StandardOutputNode = GameFlowAsset->CreateDefaultFinishNode();
		UGameFlowGraphNode* OutputGraphNode = UGameFlowNodeFactory::CreateGraphNode(StandardOutputNode, GameFlowGraph);
		OutputGraphNode->NodePosX += 300.f;

		// Mark the asset as already been opened at least one time.
		// Doing this will avoid creating duplicate default pins.
		GameFlowAsset->bHasAlreadyBeenOpened = true;
	}
}

void UGameFlowGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	Super::GetGraphContextActions(ContextMenuBuilder);
	
	// Add Custom input and output context actions.
	TSharedRef<FGameFlowNodeSchemaAction_NewNode> CustomInputAction (new FGameFlowNodeSchemaAction_NewNode(UGameFlowNode_Input::StaticClass(),INVTEXT("Node"), INVTEXT("Custom Input"),
		                                                            INVTEXT("Create a custom entry point."), 0));
	ContextMenuBuilder.AddAction(CustomInputAction);
	TSharedRef<FGameFlowNodeSchemaAction_NewNode> CustomOutputAction (new FGameFlowNodeSchemaAction_NewNode(UGameFlowNode_Output::StaticClass(),INVTEXT("Node"), INVTEXT("Custom Output"),
																	INVTEXT("Create a custom exit point."), 0));
	ContextMenuBuilder.AddAction(CustomOutputAction);
	
	// Build a context menu action for all nodes which derives from UGameFlowNode.
	for(TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* ChildClass = *ClassIt;

		// Ignore deprecated classes and old classes which have a new version.
		if(ChildClass->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists)) continue;
		
		const bool bIsInputOrOutputNodeClass = ChildClass == UGameFlowNode_Input::StaticClass() || ChildClass == UGameFlowNode_Output::StaticClass();
        const bool bIsChildClass = ChildClass->IsChildOf(UGameFlowNode::StaticClass()) && ChildClass != UGameFlowNode::StaticClass();
		// List of conditions a class needs to meet in order to appear in contextual menu:
		// 1. Select only classes which are children of UGameFlowNode, Base class excluded.
		// 2. Should not be an input or output node classes, we already have actions for these.
		if(bIsChildClass && !bIsInputOrOutputNodeClass)
		{
			TSharedRef<FGameFlowNodeSchemaAction_NewNode> NewNodeAction(new FGameFlowNodeSchemaAction_NewNode(ChildClass, INVTEXT("Node"), FText::FromString(ChildClass->GetDescription()),
																										  FText::FromString(ChildClass->GetName()), 0));
			ContextMenuBuilder.AddAction(NewNodeAction);
		}
	}
}

