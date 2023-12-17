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
		UGameFlowNode_Input* StandardInputNode = CreateDefaultInputs(*GameFlowGraph);
		UGameFlowGraphNode* InputGraphNode = UGameFlowNodeFactory::CreateGraphNode(StandardInputNode, GameFlowGraph);
        GameFlowGraph->RootNodes.Add(InputGraphNode);
		
		// Create standard output node.
		UGameFlowNode_Output* StandardOutputNode = CreateDefaultOutputs(*GameFlowGraph);
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
			TSharedRef<FGameFlowNodeSchemaAction_NewNode> NewNodeAction(new FGameFlowNodeSchemaAction_NewNode(ChildClass, INVTEXT("Node"),
			                                                                                          FText::FromString(ChildClass->GetDescription()),
			                                                                                          FText::FromString(ChildClass->GetName()), 0));
			ContextMenuBuilder.AddAction(NewNodeAction);
		}
	}
}

bool UGameFlowGraphSchema::CompileGraph(UGameFlowGraph& Graph, UGameFlowAsset* GameFlowAsset) const
{
	UE_LOG(LogGameFlow, Display, TEXT("Compiling Game Flow Asset: %s..."), *GameFlowAsset->GetName());
	bool bCompilationSuccessful = false;
	for(UGameFlowGraphNode* Node : Graph.RootNodes)
	{
		UE_LOG(LogGameFlow, Display, TEXT("%s: Compiling from root: %s"), *GameFlowAsset->GetName(),  *Node->GetNodeAsset()->GetName());
		bCompilationSuccessful = CompileGraphBranch(Node, GameFlowAsset);
	}
	return bCompilationSuccessful;
}

bool UGameFlowGraphSchema::CompileGraphBranch(UGameFlowGraphNode* RootNode, UGameFlowAsset* GameFlowAsset) const 
{
	const UGameFlowNode* NodeAsset = RootNode->GetNodeAsset();
	bool bBranchCompilationSuccessful = true;
	// Branch compilation can only start from input node assets.
	if(NodeAsset->IsA(UGameFlowNode_Input::StaticClass()))
	{
		// Start compiling from a graph input node.
		TQueue<UGameFlowGraphNode*> ToCompile;
		ToCompile.Enqueue(RootNode);

		// Keep going until we've compiled all graph nodes.
		while(!ToCompile.IsEmpty())
		{
			// Pick a new node to compile
			UGameFlowGraphNode* CurrentNode = nullptr;
			ToCompile.Dequeue(CurrentNode);
        
			UGameFlowNode* SourceNode = CurrentNode->GetNodeAsset();
			SourceNode->Outputs.Empty();
		
			for(const UEdGraphPin* Pin : CurrentNode->Pins)
			{
				// Check the links for all output pins.
				if(Pin->HasAnyConnections() && Pin->Direction == EGPD_Output)
				{
					UGameFlowGraphNode* DestinationNode = CastChecked<UGameFlowGraphNode>(Pin->LinkedTo[0]->GetOwningNode());
					const UEdGraphPin* DestinationPin = Pin->LinkedTo[0];
				    
					const FGameFlowPinNodePair DestinationPinNameAndNode(DestinationPin->PinName, DestinationNode->GetNodeAsset());
					// Update node asset with graph connections.
					SourceNode->AddOutput(Pin->PinName, DestinationPinNameAndNode);
				
					// Put the destination node inside the queue, it's the next
					// we're going to compile.
					ToCompile.Enqueue(DestinationNode);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogGameFlow, Warning, TEXT("%s is not an input node, therefore branch is invalid! Branch compilation "
									"must start from a node of type UGameFlowNode_Input"), *NodeAsset->GetName());
	}

	return bBranchCompilationSuccessful;
}

void UGameFlowGraphSchema::RecreateBranchConnections(const UGameFlowGraph& Graph, const UGameFlowNode* RootNodeAsset) const
{
	TQueue<UGameFlowGraphNode*> ToRebuild;
	UGameFlowGraphNode* CurrentNode = Graph.GraphNodes.FindRef(RootNodeAsset->GetUniqueID());
	// Start rebuilding graph from an input node.
	ToRebuild.Enqueue(CurrentNode);
	
	// As long as there are nodes to build, keep iterating.
	while(!ToRebuild.IsEmpty())
	{
		// Pick the next node to rebuild.
		ToRebuild.Dequeue(CurrentNode);
		UGameFlowNode* CurrentNodeAsset = CurrentNode->GetNodeAsset();
		
		for(const FName& OutPinName : CurrentNodeAsset->GetOutputPins())
		{
			// Find the node and pin to which the current node is connected to.
			auto Pair = CurrentNodeAsset->GetNextNode(OutPinName);
			const FName& InPinName = Pair.InputPinName;
			const UGameFlowNode* NextNode = Pair.Node;
			
			// If next node is invalid or input pin name is None, skip the iteration.
			if(NextNode == nullptr || InPinName.IsEqual(EName::None)) continue;
			
			// Create the graph node for the connected node.
			UGameFlowGraphNode* GraphNode = Graph.GraphNodes.FindRef(NextNode->GetUniqueID());
			
			UEdGraphPin* InPin = GraphNode->FindPin(InPinName);
			UEdGraphPin* OutPin = CurrentNode->FindPin(OutPinName);
			// After finding the current node output pin and the next node input pin,
			// create a connection between the two.
			TryCreateConnection(OutPin, InPin);
			
			// Enqueue next node, we'll need to rebuild it.
			ToRebuild.Enqueue(GraphNode);
		}
	}
}

UGameFlowNode_Input* UGameFlowGraphSchema::CreateDefaultInputs(UGameFlowGraph& Graph) const
{
	UGameFlowAsset* GameFlowAsset = Graph.GameFlowAsset;
	// Initialize standard input.
	const FName StandardInputName = "Start";
	UGameFlowNode_Input* StandardInputNode = NewObject<UGameFlowNode_Input>(GameFlowAsset, "GameFlowAsset.StartNode");

	// Update Game Flow asset with new default input node.
	GameFlowAsset->CustomInputs.Add(StandardInputName, StandardInputNode);
	GameFlowAsset->Nodes.Add(StandardInputNode->GetUniqueID(), StandardInputNode);
	
	return StandardInputNode;
}

UGameFlowNode_Output* UGameFlowGraphSchema::CreateDefaultOutputs(UGameFlowGraph& Graph) const
{
	UGameFlowAsset* GameFlowAsset = Graph.GameFlowAsset;
	// Initialize standard input.
	const FName StandardInputName = "Start";
	UGameFlowNode_Output* StandardOutputNode = NewObject<UGameFlowNode_Output>(GameFlowAsset, "GameFlowAsset.FinishNode");

	// Update Game Flow asset with new default output node.
	GameFlowAsset->CustomOutputs.Add(StandardInputName, StandardOutputNode);
	GameFlowAsset->Nodes.Add(StandardOutputNode->GetUniqueID(), StandardOutputNode);
	return StandardOutputNode;
}

