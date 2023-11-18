// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowGraphSchema.h"

#include "GameFlowEditor.h"
#include "Asset/Graph/GameFlowConnectionDrawingPolicy.h"
#include "Asset/Graph/GameFlowNodeSchemaAction_NewNode.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Utils/GameFlowFactory.h"
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
	UE_LOG(LogGameFlow, Display, TEXT("Creating default nodes..."));
	
	UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(&Graph);
	if(GameFlowGraph != nullptr)
	{
		UGameFlowAsset* GameFlowAsset = GameFlowGraph->GameFlowAsset;
		
		// Create standard input node.
		UGameFlowNode* StandardInputNode = GameFlowAsset->CustomInputs["Start"];
		UGameFlowNodeFactory::CreateGraphNode(StandardInputNode, GameFlowGraph);

		// Create standard output node.
		UGameFlowNode* StandardOutputNode = GameFlowAsset->CustomOutputs["Finish"];
		UGameFlowGraphNode* OutputGraphNode = UGameFlowNodeFactory::CreateGraphNode(StandardOutputNode, GameFlowGraph);
		OutputGraphNode->NodePosX += 300.f;
	}
}

void UGameFlowGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	Super::GetGraphContextActions(ContextMenuBuilder);
	
	// Add Custom input and output context actions.
	TSharedRef<FGameFlowNodeSchemaAction_NewNode> CustomInputAction (new FGameFlowNodeSchemaAction_NewNode(UGameFlowNode::StaticClass(),INVTEXT("Node"), INVTEXT("Custom Input"),
		                                                            INVTEXT("Create a custom entry point."), 0));
	ContextMenuBuilder.AddAction(CustomInputAction);
	TSharedRef<FGameFlowNodeSchemaAction_NewNode> CustomOutputAction (new FGameFlowNodeSchemaAction_NewNode(UGameFlowNode::StaticClass(),INVTEXT("Node"), INVTEXT("Custom Output"),
																	INVTEXT("Create a custom exit point."), 0));
	ContextMenuBuilder.AddAction(CustomOutputAction);
}

