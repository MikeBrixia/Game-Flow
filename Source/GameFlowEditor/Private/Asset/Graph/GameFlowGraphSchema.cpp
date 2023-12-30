﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowGraphSchema.h"
#include "GameFlowEditor.h"
#include "Asset/Graph/GameFlowConnectionDrawingPolicy.h"
#include "Asset/Graph/GameFlowNodeSchemaAction_NewNode.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Nodes/GameFlowNode_Dummy.h"
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

	// True if both pins are valid and requested connection is not recursive, false otherwise.
	const bool bValidAndNotRecursive = A != nullptr && B != nullptr && A->GetOwningNode() != B->GetOwningNode();
	if(bValidAndNotRecursive)
	{
		const bool bFromInputToOutput = A->Direction == EGPD_Input && B->Direction == EGPD_Output;
		const bool bFromOutputToInput = A->Direction == EGPD_Output && B->Direction == EGPD_Input;
		// Allow only connections between one output and input pins.
		if (bFromInputToOutput || bFromOutputToInput)
		{
			ConnectionResponse.Response = CONNECT_RESPONSE_MAKE;
			ConnectionResponse.Message = INVTEXT("Node connection allowed");
		}
		else
		{
			ConnectionResponse.Response = CONNECT_RESPONSE_DISALLOW;
			ConnectionResponse.Message = INVTEXT("Node connection is not allowed");
		}
	}
	// Otherwise, just reject it.
	else
	{
		ConnectionResponse.Response = CONNECT_RESPONSE_DISALLOW;
		ConnectionResponse.Message = INVTEXT("Connections refused: one or both pins are invalid(nullptr)");
	}
	
	return ConnectionResponse;
}

bool UGameFlowGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	const bool bConnectionCreated = Super::TryCreateConnection(A, B);
	
	if(bConnectionCreated)
	{
		UGameFlowNode* A_NodeAsset = CastChecked<UGameFlowGraphNode>(A->GetOwningNode())->GetNodeAsset();
		UGameFlowNode* B_NodeAsset = CastChecked<UGameFlowGraphNode>(B->GetOwningNode())->GetNodeAsset();
		const UGameFlowAsset* GameFlowAsset = A_NodeAsset->GetTypedOuter<UGameFlowAsset>();

		// Update node asset on the two pins.
		A->DefaultObject = B_NodeAsset;
		B->DefaultObject = A_NodeAsset;
		
		// Is the asset enabled for live compile?
		if(GameFlowAsset->bLiveCompile)
		{
			switch(A->Direction)
			{
			default: break;

			case EGPD_Input:
				{
					ConnectNodes(B_NodeAsset, B->PinName, A_NodeAsset, A->PinName);
					break;
				}
			
			case EGPD_Output:
				{
					ConnectNodes(A_NodeAsset, A->PinName, B_NodeAsset, B->PinName);
					break;
				}
			}
		}
	}
	
	return bConnectionCreated;
}

void UGameFlowGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	Super::BreakSinglePinLink(SourcePin, TargetPin);
	
	const UGameFlowGraphNode* A_Node = CastChecked<UGameFlowGraphNode>(SourcePin->GetOwningNode());
	const UGameFlowGraph* Graph = CastChecked<UGameFlowGraph>(A_Node->GetGraph());
	if (Graph->GameFlowAsset->bLiveCompile)
	{
		UGameFlowNode* A_NodeAsset = A_Node->GetNodeAsset();
		UGameFlowNode* B_NodeAsset = CastChecked<UGameFlowGraphNode>(TargetPin->GetOwningNode())->GetNodeAsset();
		switch (SourcePin->Direction)
		{
		default: break;

		case EGPD_Input:
			{
				B_NodeAsset->RemoveOutput(TargetPin->PinName);
				break;
			}

		case EGPD_Output:
			{
				A_NodeAsset->RemoveOutput(SourcePin->PinName);
				break;
			}
		}
	}
	
}

void UGameFlowGraphSchema::ConnectNodes(UGameFlowNode* NodeAsset_A, const FName& PinNameA,
	UGameFlowNode* NodeAsset_B, const FName& PinNameB) const
{
	NodeAsset_A->AddOutput(PinNameA, FGameFlowPinNodePair(PinNameB, NodeAsset_B));
	NodeAsset_B->AddInput(PinNameB, FGameFlowPinNodePair(PinNameA, NodeAsset_A));
}

void UGameFlowGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	// Break all target pin connections.
	for(UEdGraphPin* LinkedPin : TargetPin.LinkedTo)
	{
		BreakSinglePinLink(&TargetPin, LinkedPin);
	}

	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UGameFlowGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(&Graph);
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

UEdGraphNode* UGameFlowGraphSchema::CreateSubstituteNode(UEdGraphNode* Node, const UEdGraph* Graph,
                                                         FObjectInstancingGraph* InstanceGraph, TSet<FName>& InOutExtraNames) const
{
	return Super::CreateSubstituteNode(Node, Graph, InstanceGraph, InOutExtraNames);
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

TArray<UGameFlowGraphNode*> UGameFlowGraphSchema::GetGraphOrphanNodes(const UGameFlowGraph& Graph) const
{
	TArray<UGameFlowGraphNode*> OrphanNodes;
	// Find orphan nodes.
	for(UGameFlowGraphNode* GraphNode : reinterpret_cast<const TArray<TObjectPtr<UGameFlowGraphNode>>&>(Graph.Nodes))
	{
		// Array of all the node input pins who have at least one connection.
		TArray<UEdGraphPin*> ConnectedInputPins = GraphNode->Pins.FilterByPredicate([] (const UEdGraphPin* Pin)
		{
			return Pin->Direction == EGPD_Input && Pin->HasAnyConnections();
		});

		// If none of the node input pins have any connection,
		// mark it as an orphan node.
		if(ConnectedInputPins.Num() == 0)
		{
    		OrphanNodes.Add(GraphNode);
		}
	}

	return OrphanNodes;
}

ENodeValidationResult UGameFlowGraphSchema::ValidateAsset(UGameFlowGraph& Graph) const
{
	ENodeValidationResult ValidationResult = ENodeValidationResult::Success;
	TArray<UGameFlowGraphNode*> GraphNodes = reinterpret_cast<TArray<TObjectPtr<UGameFlowGraphNode>>&>(Graph.Nodes);
	// Validate all the graph nodes.
	for(UGameFlowGraphNode* GraphNode : GraphNodes)
	{
		ValidationResult = ValidateNodeAsset(Graph, GraphNode);
		if(ValidationResult == ENodeValidationResult::Error) break;
	}

	return ValidationResult;
}

ENodeValidationResult UGameFlowGraphSchema::ValidateNodeAsset(UGameFlowGraph& Graph, UGameFlowGraphNode* GraphNode) const
{
	ENodeValidationResult ValidationResult = ENodeValidationResult::Success;
	const UClass* NodeClass = GraphNode->GetNodeAsset()->StaticClass();
	const FString ClassName = NodeClass->GetName();

	const UClass* DummyNodeClass = UGameFlowNode_Dummy::StaticClass();
	
	// When a node is of a class which has been marked as 'Abstract', proceed
	// by removing all associated instances of this class from the Game Flow asset.
	// Ideally, all old abstract node instances will be replaced by a dummy node
	// with the only purpose of keeping the asset functional.
	if(NodeClass->HasAnyClassFlags(CLASS_Abstract))
	{
		ValidationResult = ENodeValidationResult::Error;
		UE_LOG(LogGameFlow, Error, TEXT("%s class is abstract! all instances of this class will be removed from Game Flow assets and replaced"
								  "with dummy node of class: %s."), *ClassName, *DummyNodeClass->GetName());
	}
	
	// When a node has been marked as deprecated, log a warning to the console
	// and inform users they should replace or remove that node in the near future.
	if(NodeClass->HasAnyClassFlags(CLASS_Deprecated))
	{
		ValidationResult = ENodeValidationResult::Warning;
		const FString DeprecationMessage = NodeClass->GetMetaData("DeprecationMessage");
		UE_LOG(LogGameFlow, Warning, TEXT("%s class has been deprecated! %s"), *ClassName, *DeprecationMessage);
	}

	return ValidationResult;
}

bool UGameFlowGraphSchema::CompileGraph(const UGameFlowGraph& Graph, UGameFlowAsset* GameFlowAsset) const
{
	UE_LOG(LogGameFlow, Display, TEXT("Compiling Game Flow Asset: %s..."), *GameFlowAsset->GetName());
	
	bool bBranchCompilationSuccessful = true;
	// Compile all graph branches.
	for(UGameFlowGraphNode* Node : Graph.RootNodes)
	{
		UE_LOG(LogGameFlow, Display, TEXT("%s: Compiling from root: %s"), *GameFlowAsset->GetName(),  *Node->GetNodeAsset()->GetName());
		bBranchCompilationSuccessful = CompileGraphBranch(Node);
	}

	// An array of nodes which does not have any parent(input pins do not hold any connection).
	TArray<UGameFlowGraphNode*> OrphanNodes = GetGraphOrphanNodes(Graph);
	bool bOrphansCompilationSuccessful = true;
	
    // Clear all orphan nodes from game flow asset, we'll repopulate it during orphan compilation.
	GameFlowAsset->OrphanNodes.Empty();
	// Compile all orphan nodes by assuming that each of them is the root of a branch.
	for(UGameFlowGraphNode* OrphanNode : OrphanNodes)
	{
		UE_LOG(LogGameFlow, Display, TEXT("%s: Compiling from orphan: %s"), *GameFlowAsset->GetName(),  *OrphanNode->GetNodeAsset()->GetName());
		bOrphansCompilationSuccessful = CompileGraphBranch(OrphanNode);

		UGameFlowNode* NodeAsset = OrphanNode->GetNodeAsset();
		// Notify the asset that during compilation we've found a new orphan node.
		// N.B. Input nodes does not counts as orphan nodes.
		if(!NodeAsset->IsA(UGameFlowNode_Input::StaticClass()))
		{
			GameFlowAsset->OrphanNodes.Add(NodeAsset);
		}
	}
	
	return bBranchCompilationSuccessful && bOrphansCompilationSuccessful;
}

bool UGameFlowGraphSchema::CompileGraphBranch(UGameFlowGraphNode* RootNode) const
{
	bool bBranchCompilationSuccessful = true;

	// Start compiling from a graph input node.
	TQueue<UGameFlowGraphNode*> ToCompile;
	ToCompile.Enqueue(RootNode);

	// Keep going until we've compiled all graph nodes.
	while (!ToCompile.IsEmpty())
	{
		// Pick a new node to compile
		UGameFlowGraphNode* CurrentNode = nullptr;
		ToCompile.Dequeue(CurrentNode);

		UGameFlowNode* SourceNodeAsset = CurrentNode->GetNodeAsset();
		SourceNodeAsset->Outputs.Empty();

		for (const UEdGraphPin* Pin : CurrentNode->Pins)
		{
			// Check the links for all output pins.
			if (Pin->HasAnyConnections() && Pin->Direction == EGPD_Output)
			{
				UGameFlowGraphNode* DestinationNode = CastChecked<UGameFlowGraphNode>(Pin->LinkedTo[0]->GetOwningNode());
				UGameFlowNode* DestinationNodeAsset = DestinationNode->GetNodeAsset();
				const UEdGraphPin* DestinationPin = Pin->LinkedTo[0];
				
				// Connect the source and destination node.
				ConnectNodes(SourceNodeAsset, Pin->PinName, DestinationNodeAsset, DestinationPin->PinName);
				// Put the destination node inside the queue, it's the next we're going to compile.
				ToCompile.Enqueue(DestinationNode);
			}
		}
	}

	return bBranchCompilationSuccessful;
}

bool UGameFlowGraphSchema::CompileGraphNode(UGameFlowGraphNode* GraphNode, const TArray<EEdGraphPinDirection> Directions) const
{
	UGameFlowNode* NodeAsset = GraphNode->GetNodeAsset();
	NodeAsset->Outputs.Empty();

	bool bCompileSuccessful = true;
	for(const UEdGraphPin* Pin : GraphNode->Pins)
	{
		// If current pin does not have any connections, skip to the next iteration.
		if(!Pin->HasAnyConnections()) continue;
		
		const EEdGraphPinDirection PinDirection = Pin->Direction;
		if(PinDirection == EGPD_Output && Directions.Contains(EGPD_Output))
		{
			UGameFlowNode* ConnectedNode = CastChecked<UGameFlowNode>(Pin->DefaultObject);
			FName& ConnectedPinName = Pin->LinkedTo[0]->PinName;
			ConnectNodes(NodeAsset, Pin->PinName, ConnectedNode, ConnectedPinName);
		}
		else if(PinDirection == EGPD_Input && Directions.Contains(EGPD_Input))
		{
			const UEdGraphPin* InputPin = Pin->LinkedTo[0];
			UGameFlowNode* InputNodeAsset = CastChecked<UGameFlowGraphNode>(InputPin->GetOwningNode())->GetNodeAsset();
			// Compile recursively all output pins on the input node. N.B. we should find a better solution, but for now
			// it works.
			ConnectNodes(InputNodeAsset, InputPin->PinName, NodeAsset, Pin->PinName);
		}
	}
	
	return bCompileSuccessful;
}

void UGameFlowGraphSchema::RecreateGraphNodesConnections(const UGameFlowGraph& Graph) const
{
	// Recreate node connections starting from each root.
	for(UGameFlowGraphNode* RootNode : Graph.RootNodes)
	{
		RecreateBranchConnections(Graph, RootNode);
	}
	
	TArray<UGameFlowGraphNode*> OrphanNodes = GetGraphOrphanNodes(Graph);
	// Treat all asset orphan nodes as branch roots and recreate connections.
	for(UGameFlowGraphNode* OrphanNode : OrphanNodes)
	{
		RecreateBranchConnections(Graph, OrphanNode);
	}
}

void UGameFlowGraphSchema::RecreateBranchConnections(const UGameFlowGraph& Graph, UGameFlowGraphNode* RootNode) const
{
	TQueue<UGameFlowGraphNode*> ToRebuild;
	UGameFlowGraphNode* CurrentNode = RootNode;
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

void UGameFlowGraphSchema::RecreateNodeConnections(const UGameFlowGraph& Graph, UGameFlowGraphNode* GraphNode,
                                                   const TArray<EEdGraphPinDirection> Directions) const
{
	for (UEdGraphPin* Pin : GraphNode->Pins)
	{
		if (!Directions.Contains(Pin->Direction)) continue;

		const UGameFlowNode* NodeAsset = GraphNode->GetNodeAsset();
		// Find the node and pin to which the current node is connected to.
		FGameFlowPinNodePair Pair = Pin->Direction == EGPD_Output
			            ? NodeAsset->GetNextNode(Pin->PinName)
			            : NodeAsset->Inputs.FindRef(Pin->PinName);
		const FName& InPinName = Pair.InputPinName;
		const UGameFlowNode* ConnectedNode = Pair.Node;
        
		// If next node is invalid or input pin name is None, skip the iteration.
		if (ConnectedNode == nullptr || InPinName.IsEqual(EName::None)) continue;

		// Create the graph node for the connected node.
		const UGameFlowGraphNode* ConnectedGraphNode = Graph.GraphNodes.FindRef(ConnectedNode->GetUniqueID());
		
		UEdGraphPin* OtherPin = ConnectedGraphNode->FindPin(InPinName);
		// After finding the current node output pin and the next node input pin,
		// create a connection between the two.
		TryCreateConnection(Pin, OtherPin);
	}
}


