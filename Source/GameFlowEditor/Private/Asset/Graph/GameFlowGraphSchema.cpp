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
					A_NodeAsset->AddInput(A->PinName, FGameFlowPinNodePair(B->PinName, B_NodeAsset));
					break;
				}
			
			case EGPD_Output:
				{
					A_NodeAsset->AddOutput(A->PinName, FGameFlowPinNodePair(B->PinName, B_NodeAsset));
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
	UGameFlowNodeFactory::CreateGraphNode(StandardInputNode, GameFlowGraph);
	
	// Create standard output node.
	UGameFlowNode_Output* StandardOutputNode = CreateDefaultOutputs(*GameFlowGraph);
	UGameFlowGraphNode* OutputGraphNode = UGameFlowNodeFactory::CreateGraphNode(StandardOutputNode, GameFlowGraph);
	OutputGraphNode->NodePosX += 300.f;

	// Mark the asset as already been opened at least one time.
	// Doing this will avoid creating duplicate default nodes.
	GameFlowAsset->bHasAlreadyBeenOpened = true;
}

UEdGraphNode* UGameFlowGraphSchema::CreateSubstituteNode(UEdGraphNode* Node, const UEdGraph* Graph,
	FObjectInstancingGraph* InstanceGraph, TSet<FName>& InOutExtraNames) const
{
	const UGameFlowGraphNode* GraphNode = CastChecked<UGameFlowGraphNode>(Node);
    UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(GraphNode->GetGraph());
	UGameFlowNode* SubstituteNodeAsset = CastChecked<UGameFlowNode>(InstanceGraph->GetDestinationObject(GraphNode->GetNodeAsset()));

	// The number of input pins found on the node we want to substitute.
	const int32 CurrentInputPinsNum = GraphNode->Pins.FilterByPredicate([](const UEdGraphPin* PinObj)
	{
		return PinObj->Direction == EGPD_Input;
	}).Num();
	
	// The number of output pins found on the node we wanto to substitute
	const int32 CurrentOutputPinsNum = GraphNode->Pins.FilterByPredicate([](const UEdGraphPin* PinObj)
	{
		return PinObj->Direction == EGPD_Output;
	}).Num();
	
	UGameFlowGraphNode* SubstituteNode = UGameFlowNodeFactory::CreateGraphNode(SubstituteNodeAsset, GameFlowGraph);
	// Place the new node at the same position of the old one.
	SubstituteNode->NodePosX = Node->NodePosX;
	SubstituteNode->NodePosY = Node->NodePosY;
	
	// Rewire pins inside substitute node to match source node.
	for(UEdGraphPin* Pin : Node->Pins)
	{
		const int32 SubstituteInputPinsNum = SubstituteNodeAsset->GetInputPins().Num();
		const int32 SubstituteOutputPinsNum = SubstituteNodeAsset->GetOutputPins().Num();
		
		UEdGraphPin* SubstituteNodePin = SubstituteNode->FindPin(Pin->PinName);
		// Have we found a pin with the same name in the substitute node?
		// If not, try creating it.
		if(SubstituteNodePin == nullptr)
		{
			bool bShouldAddPin = false;
			// Does this node have variable pins depending on the direction?
			if(Pin->Direction == EGPD_Input)
			{
				bShouldAddPin = SubstituteNodeAsset->bCanAddInputPin
				                 && CurrentInputPinsNum > SubstituteInputPinsNum;
			}
			else if(Pin->Direction == EGPD_Output)
			{
				bShouldAddPin = SubstituteNodeAsset->bCanAddOutputPin
				                && CurrentOutputPinsNum > SubstituteOutputPinsNum;
			}
			
			// If we've determined that no pin should be added, skip the iteration.
			if(!bShouldAddPin) continue;
			
			// Create new substitute node pin.
			SubstituteNodePin = SubstituteNode->CreateNodePin(Pin->Direction);
		}
		
		// If source pin had a connection, recreate it on the substitute node.
		if(Pin->HasAnyConnections())
		{
			SubstituteNodePin->DefaultObject = Pin->DefaultObject;
			for(UEdGraphPin* ConnectedPin : Pin->LinkedTo)
			{
				// Here we call superclass implementation because we want to
				// avoid unnecessary live compilation features offered by game flow schema.
				Super::TryCreateConnection(SubstituteNodePin, ConnectedPin);
			}
		}
	}
	return SubstituteNode;
}

void UGameFlowGraphSchema::AlignNodeAssetToGraphNode(UGameFlowGraphNode* GraphNode) const
{
	UGameFlowNode* NodeAsset = GraphNode->GetNodeAsset();
	// Ensure there node asset ports are aligned with graph node pins connections.
	for(const UEdGraphPin* Pin : GraphNode->Pins)
	{
		if(!Pin->HasAnyConnections())
		{
			if(Pin->Direction == EGPD_Input)
			{
				NodeAsset->RemoveInputPort(Pin->PinName);
			}
			else if(Pin->Direction == EGPD_Output)
			{
				NodeAsset->RemoveOutputPort(Pin->PinName);
			}
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
	GameFlowAsset->Nodes.Add(StandardInputNode);
	
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
	GameFlowAsset->Nodes.Add(StandardOutputNode);
	return StandardOutputNode;
}

void UGameFlowGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	Super::GetGraphContextActions(ContextMenuBuilder);
	
	// Add Custom input and output context actions.
	TSharedRef<FGameFlowNodeSchemaAction_NewNode> CustomInputAction (new FGameFlowNodeSchemaAction_NewNode(UGameFlowNode_Input::StaticClass(),INVTEXT("I/O"), INVTEXT("Custom Input"),
		                                                            INVTEXT("Create a custom entry point."), 1));
	ContextMenuBuilder.AddAction(CustomInputAction);
	TSharedRef<FGameFlowNodeSchemaAction_NewNode> CustomOutputAction (new FGameFlowNodeSchemaAction_NewNode(UGameFlowNode_Output::StaticClass(),INVTEXT("I/O"), INVTEXT("Custom Output"),
																	INVTEXT("Create a custom exit point."), 1));
	ContextMenuBuilder.AddAction(CustomOutputAction);
	
	// Build a context menu action for all nodes which derives from UGameFlowNode.
	for(TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* ChildClass = *ClassIt;
		// Create context menu actions only for nodes who respects game flow criteria
		if(CanCreateGraphNodeForClass(ChildClass))
		{
			FText ClassCategory = ChildClass->GetMetaDataText("Category");
			// If node class has no defined category, use the default one.
			ClassCategory = ClassCategory.IsEmptyOrWhitespace()? INVTEXT("Default") : ClassCategory;
			
			// Add context menu action for creating a new node of this class.
			TSharedRef<FGameFlowNodeSchemaAction_NewNode> NewNodeAction(new FGameFlowNodeSchemaAction_NewNode(ChildClass, ClassCategory,
															   FText::FromString(ChildClass->GetDescription()), ChildClass->GetToolTipText(),0));
			ContextMenuBuilder.AddAction(NewNodeAction);
		}
	}
}

void UGameFlowGraphSchema::ValidateAsset(UGameFlowGraph& Graph) const
{
	TArray<UGameFlowGraphNode*> GraphNodes = reinterpret_cast<TArray<TObjectPtr<UGameFlowGraphNode>>&>(Graph.Nodes);
	// Validate all the graph nodes.
	for(UGameFlowGraphNode* GraphNode : GraphNodes)
	{
		ValidateNodeAsset(GraphNode);
	}
}

void UGameFlowGraphSchema::ValidateNodeAsset(UGameFlowGraphNode* GraphNode) const
{
	UGameFlowNode* NodeAsset = GraphNode->GetNodeAsset();
	const UClass* NodeClass = NodeAsset->GetClass();
	const FString ClassName = NodeClass->GetName();
	UClass* DummyNodeClass = UGameFlowNode_Dummy::StaticClass();
	
	GraphNode->bHasCompilerMessage = false;
	NodeAsset->ValidateAsset();
	AlignNodeAssetToGraphNode(GraphNode);
	// When node is a dummy just throw an error message
	if(NodeClass->IsChildOf(DummyNodeClass) || NodeClass == DummyNodeClass)
	{
		GraphNode->ReportError(EMessageSeverity::Error);
	}
	else // Otherwise go through all the validation process.
	{
		// When a node is of a class which has been marked as 'Abstract', proceed
		// by replacing all associated instances of this class from the Game Flow asset.
		if(NodeClass->HasAnyClassFlags(CLASS_Abstract))
		{
			UE_LOG(LogGameFlow, Error, TEXT("%s class is abstract! all instances of this class will be removed from Game Flow assets and replaced"
									  "with dummy node of class: %s."), *ClassName, *DummyNodeClass->GetName());
			SubstituteWithDummyNode(GraphNode, DummyNodeClass);
			GraphNode->ReportError(EMessageSeverity::Error);
		}
	
		// When a node has been marked as deprecated, log a warning to the console
		// and inform users they should replace or remove that node in the near future.
		if(NodeClass->HasAnyClassFlags(CLASS_Deprecated))
		{
			const FString DeprecationMessage = NodeClass->GetMetaData("DeprecationMessage");
			UE_LOG(LogGameFlow, Warning, TEXT("%s class has been deprecated! %s"), *ClassName, *DeprecationMessage);
			GraphNode->ReportError(EMessageSeverity::Warning);
		}
	}
}

bool UGameFlowGraphSchema::CanCreateGraphNodeForClass(UClass* Class) const
{
	const bool bIsSkellClass = Class->GetName().StartsWith("SKEL_", ESearchCase::CaseSensitive);
	// True if this class cannot be instanced inside the graph, false otherwise.
	const bool bNotInstanceable = Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_Abstract | CLASS_Hidden) || bIsSkellClass;
	
	// True only if this class is a child of UGameFlowNode, otherwise false.
	const bool bIsChildClass = Class->IsChildOf(UGameFlowNode::StaticClass()) && Class != UGameFlowNode::StaticClass();
	// True if this class is or derives from game flow dummy nodes.
	const bool bIsDummyClass = Class->IsChildOf(UGameFlowNode_Dummy::StaticClass()) && Class == UGameFlowNode_Dummy::StaticClass();
	const bool bIsInputOrOutputClass = Class == UGameFlowNode_Input::StaticClass() || Class == UGameFlowNode_Output::StaticClass();
    // True if the given class does not belong to a category of classes which could not be directly instanced by the user.
	const bool bUserInstancedAllowed = bIsChildClass && !bIsDummyClass && !bIsInputOrOutputClass;

	return bUserInstancedAllowed && !bNotInstanceable;
}

void UGameFlowGraphSchema::SubstituteWithDummyNode(UGameFlowGraphNode* GraphNode, const TSubclassOf<UGameFlowNode_Dummy> DummyNodeClass) const
{
	UGameFlowAsset* GameFlowAsset = GraphNode->GetNodeAsset()->GetTypedOuter<UGameFlowAsset>();
	UGameFlowNode_Dummy* DummyNode = CastChecked<UGameFlowNode_Dummy>(UGameFlowNodeFactory::CreateGameFlowNode(DummyNodeClass, GameFlowAsset));
	// The node asset to substitute.
	UGameFlowNode* NodeAsset = GraphNode->GetNodeAsset();
	DummyNode->InputPins = NodeAsset->InputPins;
	DummyNode->OutputPins = NodeAsset->OutputPins;
	DummyNode->ReplacedNodeClass = NodeAsset->GetClass();
	
	FObjectInstancingGraph ObjectInstancingGraph;
	ObjectInstancingGraph.AddNewObject(DummyNode, NodeAsset);
	TSet<FName> InOutExtraNames;
	
	UGameFlowGraphNode* SubstituteNode = CastChecked<UGameFlowGraphNode>(
		CreateSubstituteNode(GraphNode, GraphNode->GetGraph(), &ObjectInstancingGraph, InOutExtraNames)
		);
	SubstituteNode->ErrorType = EMessageSeverity::Error;
	
	GraphNode->DestroyNode();
	// Recompile substitute node; this action will update the actual game flow asset.
	CompileGraphNode(SubstituteNode, TArray { EGPD_Input, EGPD_Output });
    
	UE_LOG(LogGameFlow, Display, TEXT("Node substituted with dummy"))
}

bool UGameFlowGraphSchema::CompileGraph(const UGameFlowGraph& Graph, UGameFlowAsset* GameFlowAsset) const
{
	UE_LOG(LogGameFlow, Display, TEXT("Compiling Game Flow Asset: %s..."), *GameFlowAsset->GetName());
	
	bool bBranchCompilationSuccessful = true;
	TArray<UGameFlowGraphNode*> RootNodes = Graph.GetNodesOfClass(UGameFlowNode_Input::StaticClass());
	// Compile all graph branches.
	for(UGameFlowGraphNode* Node : RootNodes)
	{
		UE_LOG(LogGameFlow, Display, TEXT("%s: Compiling from root: %s"), *GameFlowAsset->GetName(),  *Node->GetNodeAsset()->GetName());
		bBranchCompilationSuccessful = CompileGraphBranch(Node);
	}

	// An array of nodes which does not have any parent(input pins do not hold any connection).
	TArray<UGameFlowGraphNode*> OrphanNodes = Graph.GetOrphanNodes();
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
				SourceNodeAsset->AddOutput(Pin->PinName, FGameFlowPinNodePair(DestinationPin->PinName, DestinationNodeAsset));
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
		
		UGameFlowNode* ConnectedNode = CastChecked<UGameFlowNode>(Pin->DefaultObject);
		const UEdGraphPin* ConnectedPin = Pin->LinkedTo[0];
		const EEdGraphPinDirection PinDirection = Pin->Direction;
		// Compile output pin...
		if(PinDirection == EGPD_Output && Directions.Contains(EGPD_Output))
		{
			NodeAsset->AddOutput(Pin->PinName, FGameFlowPinNodePair(ConnectedPin->PinName, ConnectedNode));
		}
		// Compile input pin...
		else if(PinDirection == EGPD_Input && Directions.Contains(EGPD_Input))
		{
			NodeAsset->AddInput(Pin->PinName, FGameFlowPinNodePair(ConnectedPin->PinName, ConnectedNode));
		}
	}
	
	return bCompileSuccessful;
}

void UGameFlowGraphSchema::RecreateGraphNodesConnections(const UGameFlowGraph& Graph) const
{
	TArray<UGameFlowGraphNode*> RootNodes = Graph.GetNodesOfClass(UGameFlowNode_Input::StaticClass());
	// Recreate node connections starting from each root.
	for(UGameFlowGraphNode* RootNode : RootNodes)
	{
		RecreateBranchConnections(Graph, RootNode);
	}
	
	TArray<UGameFlowGraphNode*> OrphanNodes = Graph.GetOrphanNodes();
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
			
			// If next node is invalid or input pin name is None, skip this iteration.
			if(NextNode == nullptr || InPinName.IsEqual(EName::None)) continue;
			
			// Create the graph node for the connected node.
			UGameFlowGraphNode* GraphNode = Graph.GetGraphNodeByAsset(NextNode);
			
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

		UGameFlowNode* NodeAsset = GraphNode->GetNodeAsset();
		// Find the node and pin to which the current node is connected to.
		FGameFlowPinNodePair Pair = Pin->Direction == EGPD_Output
			            ? NodeAsset->GetNextNode(Pin->PinName)
			            : NodeAsset->Inputs.FindRef(Pin->PinName);
		const FName& InPinName = Pair.InputPinName;
		const UGameFlowNode* ConnectedNode = Pair.Node;
        
		// If next node is invalid or input pin name is None, skip the iteration.
		if (ConnectedNode == nullptr || InPinName.IsEqual(EName::None)) continue;
		
		const UGameFlowGraphNode* ConnectedGraphNode = Graph.GetGraphNodeByAsset(ConnectedNode);
		UEdGraphPin* OtherPin = ConnectedGraphNode->FindPin(InPinName);
		
		// After finding the current node output pin and the next node input pin,
		// create a connection between the two.
		bool bConnectionResult = TryCreateConnection(Pin, OtherPin);

		// If connection could not be recreated, remove asset pin logic port.
		if(!bConnectionResult)
		{
			if(Pin->Direction == EGPD_Input)
			{
				NodeAsset->RemoveInputPort(Pin->PinName);
			}
			else if(Pin->Direction == EGPD_Output)
			{
				NodeAsset->RemoveOutputPort(Pin->PinName);
			}
		}
	}
}


