// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowGraphSchema.h"
#include "GameFlowEditor.h"
#include "Asset/Graph/GameFlowConnectionDrawingPolicy.h"
#include "Asset/Graph/Actions/FGameFlowSchemaAction_ReplaceNode.h"
#include "Asset/Graph/Actions/GameFlowNodeSchemaAction_NewNode.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"

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

		const bool bValidDirection = bFromInputToOutput || bFromOutputToInput;
		const bool bAlreadyHasConnection = A->Direction == EGPD_Output && A->LinkedTo.Num() > 0;
		// Allow only connections between one output and input pins.
		if (bValidDirection && !bAlreadyHasConnection)
		{
			ConnectionResponse.Response = CONNECT_RESPONSE_MAKE;
			ConnectionResponse.Message = INVTEXT("Connection allowed");
		}
		else
		{
			ConnectionResponse.Response = CONNECT_RESPONSE_DISALLOW;
			ConnectionResponse.Message = INVTEXT("Connection not allowed");
		}
	}
	// Otherwise, just reject it.
	else
	{
		ConnectionResponse.Response = CONNECT_RESPONSE_DISALLOW;
		ConnectionResponse.Message = INVTEXT("Recursion detected: connection not allowed");
	}
	
	return ConnectionResponse;
}

void UGameFlowGraphSchema::ConnectToDefaultPin(UEdGraphPin* FromPin, UEdGraphNode* GraphNode,
	const UGameFlowGraph* Graph) const
{
	UEdGraphPin* NewNodeTargetPin = nullptr;
	// Depending on the direction, find a different target pin.
	if (FromPin->Direction == EGPD_Input)
	{
		NewNodeTargetPin = GraphNode->GetPinWithDirectionAt(0, EGPD_Output);
	}
	else if (FromPin->Direction == EGPD_Output)
	{
		NewNodeTargetPin = GraphNode->GetPinWithDirectionAt(0, EGPD_Input);
	}
	
	// Create a connection with the new node first pin.
	TryCreateConnection(FromPin, NewNodeTargetPin);
}

void UGameFlowGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(&Graph);
	UGameFlowAsset* GameFlowAsset = GameFlowGraph->GameFlowAsset;

	// Create standard input node.
	FGameFlowNodeSchemaAction_CreateOrDestroyNode::CreateNode(UGameFlowNode_Input::StaticClass(), GameFlowGraph, "Start");
	
	// Create standard output node.
	UGameFlowGraphNode* OutputGraphNode = FGameFlowNodeSchemaAction_CreateOrDestroyNode::CreateNode(UGameFlowNode_Output::StaticClass(), GameFlowGraph, "Finish");
	OutputGraphNode->NodePosX += 600.f;

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
	
	UGameFlowGraphNode* SubstituteNode = FGameFlowNodeSchemaAction_CreateOrDestroyNode::CreateNode(SubstituteNodeAsset, GameFlowGraph);
	// Place the new node at the same position of the old one.
	SubstituteNode->NodePosX = Node->NodePosX;
	SubstituteNode->NodePosY = Node->NodePosY;
	
	// If substitute node has variable pins, try to match the
	// image of the old node.
	for(UEdGraphPin* Pin : Node->Pins)
	{
		const int32 SubstituteInputPinsNum = SubstituteNodeAsset->GetInputPins().Num();
		const int32 SubstituteOutputPinsNum = SubstituteNodeAsset->GetOutputPins().Num();
		
		UEdGraphPin* SubstituteNodePin = SubstituteNode->FindPin(Pin->PinName);
		// Have we found a pin with the same name in the substitute node?
		// If not, try creating it.
		if(SubstituteNodePin == nullptr)
		{
			const bool bShouldAddInputPin = Pin->Direction == EGPD_Input && SubstituteNodeAsset->bCanAddInputPin
								            && CurrentInputPinsNum > SubstituteInputPinsNum;
			const bool bShouldAddOutputPin = Pin->Direction == EGPD_Output && SubstituteNodeAsset->bCanAddOutputPin
								             && CurrentOutputPinsNum > SubstituteOutputPinsNum;
			const bool bShouldAddPin = bShouldAddInputPin || bShouldAddOutputPin;
			
			// If no pin should be added, skip the iteration.
			if(!bShouldAddPin) continue;
			
			// Create new substitute node pin.
			SubstituteNodePin = SubstituteNode->CreateNodePin(Pin->Direction);
		}
		
		// Replace source node connections with substitute node connections.
		if(Pin->HasAnyConnections())
		{
			TArray<UEdGraphPin*> ConnectionsList = Pin->LinkedTo;
			BreakPinLinks(*Pin, true);
			for(UEdGraphPin* ConnectedPin : ConnectionsList)
			{
				TryCreateConnection(SubstituteNodePin, ConnectedPin);
			}
		}
	}
	return SubstituteNode;
}

void UGameFlowGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	Super::GetGraphContextActions(ContextMenuBuilder);
	
	// Add Custom input and output context actions.
	TSharedRef<FGameFlowNodeSchemaAction_CreateOrDestroyNode> CustomInputAction (new FGameFlowNodeSchemaAction_CreateOrDestroyNode(UGameFlowNode_Input::StaticClass(),
		                                                            INVTEXT("I/O"), INVTEXT("Custom Input"),
		                                                            INVTEXT("Create a custom entry point."), 1));
	ContextMenuBuilder.AddAction(CustomInputAction);
	TSharedRef<FGameFlowNodeSchemaAction_CreateOrDestroyNode> CustomOutputAction (new FGameFlowNodeSchemaAction_CreateOrDestroyNode(UGameFlowNode_Output::StaticClass(),
		                                                            INVTEXT("I/O"), INVTEXT("Custom Output"),
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
			TSharedRef<FGameFlowNodeSchemaAction_CreateOrDestroyNode> NewNodeAction(new FGameFlowNodeSchemaAction_CreateOrDestroyNode(ChildClass, ClassCategory,
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
	
	GraphNode->bHasCompilerMessage = false;
	GraphNode->ErrorType = 0;
	
	// When a node is of a class which has been marked as 'Abstract', proceed
	// by replacing all associated instances of this class from the Game Flow asset.
	if (NodeClass->HasAnyClassFlags(CLASS_Abstract))
	{
		UE_LOG(LogGameFlow, Error, TEXT( "%s class is abstract! all instances of this class will be invalidated and should be replaced."), *ClassName);
		GraphNode->ReportError(EMessageSeverity::Error);
	}
	
	// When a node has been marked as deprecated, log a warning to the console
	// and inform users they should replace or remove that node in the near future.
	if (NodeClass->HasAnyClassFlags(CLASS_Deprecated))
	{
		const FString DeprecationMessage = NodeClass->GetMetaData("DeprecationMessage");
		UE_LOG(LogGameFlow, Warning, TEXT("%s class has been deprecated! %s"), *ClassName, *DeprecationMessage);
		GraphNode->ReportError(EMessageSeverity::Warning);
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


