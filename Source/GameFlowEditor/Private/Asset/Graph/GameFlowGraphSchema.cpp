// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowGraphSchema.h"
#include "GameFlowEditor.h"
#include "Asset/Graph/GameFlowConnectionDrawingPolicy.h"
#include "Asset/Graph/Actions/FGameFlowSchemaAction_ReplaceNode.h"
#include "Asset/Graph/Actions/GameFlowNodeSchemaAction_NewNode.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "UObject/UObjectIterator.h"

FConnectionDrawingPolicy* UGameFlowGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID,
                                                                              float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
                                                                              UEdGraph* InGraphObj) const
{
	UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(InGraphObj);
	FGameFlowConnectionDrawingPolicy* GameFlowConnectionDrawingPolicy = new FGameFlowConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements);
	GameFlowConnectionDrawingPolicy->SetGraphObj(GameFlowGraph);
	
	return GameFlowConnectionDrawingPolicy;
}

const FPinConnectionResponse UGameFlowGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	FPinConnectionResponse ConnectionResponse;
	FText ConnectionMessage = NSLOCTEXT("FGameFlowEditor", "ConnectionMessage", "Unknown response");
	
    // If one of the two pins is invalid, do not allow connection and return.
	if(A == nullptr || B == nullptr)
	{
		ConnectionResponse.Response = CONNECT_RESPONSE_DISALLOW;
		ConnectionResponse.Message = NSLOCTEXT("FGameFlowEditor", "ConnectionMessage", "One or both of the pins are invalid!");
		UE_LOG(LogGameFlow, Display, TEXT("Connection response: %s"),
		        *ConnectionResponse.Message.ToString());
		return ConnectionResponse;
	}

	// Ensure owning nodes are valid; otherwise we must disallow connection.
	if (!IsValid(A->GetOwningNode()) || !IsValid(B->GetOwningNode()))
	{
		ConnectionResponse.Response = CONNECT_RESPONSE_DISALLOW;
		ConnectionResponse.Message = NSLOCTEXT("FGameFlowEditor", "ConnectionMessage", "The node which owns one of the two pins is invalid!");
		UE_LOG(LogGameFlow, Display, TEXT("Connection response: %s"),
		*ConnectionResponse.Message.ToString());
		return ConnectionResponse;
	}
	
	// Are we trying to establish a connection from an input pin source to an output pin destination?
	const bool bFromInputToOutput = A->Direction == EGPD_Input && B->Direction == EGPD_Output;
	// Are we trying to establish a connection from an output pin source to an input pin destination?
	const bool bFromOutputToInput = A->Direction == EGPD_Output && B->Direction == EGPD_Input;
	const bool bValidDirection = bFromInputToOutput || bFromOutputToInput;
	
	// Are we trying to connect two pins which are on the same node?
	const bool bIsRecursive = A->GetOwningNode() == B->GetOwningNode();
	// Is the source pin already connected to another pin?
	const bool bAlreadyHasConnection = (A->Direction == EGPD_Output && A->LinkedTo.Num() > 0)
	                                   || (A->Direction == EGPD_Input && B->LinkedTo.Num() > 0);
	
	// Allow only connections between one output and input pins. To be eligible for connections,
	// the source pin must have 0 connections and should not try to create a recursive connection
	// on the owning node.
	if (bValidDirection && !bAlreadyHasConnection && !bIsRecursive)
	{
		ConnectionResponse.Response = CONNECT_RESPONSE_MAKE;
		ConnectionMessage = NSLOCTEXT("FGameFlowEditor", "ConnectionMessage", "Connection allowed");
	}
	else
	{
		ConnectionResponse.Response = CONNECT_RESPONSE_DISALLOW;
		if(!bValidDirection)
		{
			ConnectionMessage = NSLOCTEXT("FGameFlowEditor", "ConnectionMessage",
			                                       "Output pins can only be connected to input pins and vice-versa");
		}
		else if(bAlreadyHasConnection)
		{
			ConnectionMessage = NSLOCTEXT("FGameFlowEditor", "ConnectionMessage",
			                              "Output pins can only have one connection");
		}
		else if(bIsRecursive)
		{
			ConnectionMessage = NSLOCTEXT("FGameFlowEditor", "ConnectionMessage",
										  "Recursion not allowed");
		}
	}
	ConnectionResponse.Message = ConnectionMessage;
	UE_LOG(LogGameFlow, Display, TEXT("Connection response: %s"),
		*ConnectionResponse.Message.ToString());
	
	return ConnectionResponse;
}

void UGameFlowGraphSchema::ConnectToDefaultPin(UEdGraphPin* FromPin, UEdGraphNode* GraphNode) const
{
	UEdGraphPin* NewNodeTargetPin = nullptr;
	// Find the default pin. Should have the opposite direction of the source pin.
	if (FromPin->Direction == EGPD_Input)
	{
		NewNodeTargetPin = GraphNode->GetPinWithDirectionAt(0, EGPD_Output);
	}
	else if (FromPin->Direction == EGPD_Output)
	{
		NewNodeTargetPin = GraphNode->GetPinWithDirectionAt(0, EGPD_Input);
	}
	
	// Create a connection with the new node default pin.
	TryCreateConnection(FromPin, NewNodeTargetPin);
}

void UGameFlowGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(&Graph);
	UGameFlowAsset* GameFlowAsset = GameFlowGraph->GameFlowAsset;

	// Create the standard input node.
	FGameFlowNodeSchemaAction_CreateOrDestroyNode::CreateNode(UGameFlowNode_Input::StaticClass(), GameFlowGraph,
		FVector2D::ZeroVector, "Start");
	
	// Create the standard output node.
	FGameFlowNodeSchemaAction_CreateOrDestroyNode::CreateNode(UGameFlowNode_Output::StaticClass(), GameFlowGraph,
		FVector2D(600, 0), "Finish");

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
	
	UGameFlowGraphNode* SubstituteNode = FGameFlowNodeSchemaAction_CreateOrDestroyNode::CreateNode(SubstituteNodeAsset,
		GameFlowGraph, FVector2D(Node->NodePosX, Node->NodePosY));
	
	// If the substitute node has variable pins, try to match the
	// image of the old node.
	for(UEdGraphPin* Pin : Node->Pins)
	{
		const int32 SubstituteInputPinsNum = SubstituteNodeAsset->GetInputPinsNames().Num();
		const int32 SubstituteOutputPinsNum = SubstituteNodeAsset->GetOutputPinsNames().Num();
		
		UEdGraphPin* SubstituteNodePin = SubstituteNode->FindPin(Pin->PinName);
		// Have we found a pin with the same name in the substitute node?
		// If not, try creating it.
		if(!IsValid(SubstituteNode))
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
				ConnectedPin->Modify();
				ConnectedPin->GetOwningNode()->Modify();
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
			// If the node class has no defined category, use the default one.
			ClassCategory = ClassCategory.IsEmptyOrWhitespace()? INVTEXT("Default") : ClassCategory;
			
			// Add a context menu action for creating a new node of this class.
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
	
	// When a node is of a class that has been marked as 'Abstract', proceed
	// by replacing all associated instances of this class from the Game Flow asset.
	if (NodeClass->HasAnyClassFlags(CLASS_Abstract))
	{
		UE_LOG(LogGameFlow, Error, TEXT( "%s class is abstract! all instances of this class will be invalidated and should be replaced."), *ClassName);
		GraphNode->ReportError(EMessageSeverity::Error, "Abstract nodes cannot be instanced");
	}
	
	// When a node has been marked as deprecated, log a warning to the console
	// and inform users they should replace or remove that node in the near future.
	if (NodeClass->HasAnyClassFlags(CLASS_Deprecated))
	{
		const FString DeprecationMessage = NodeClass->GetMetaData("DeprecationMessage");
		UE_LOG(LogGameFlow, Warning, TEXT("%s class has been deprecated! %s"), *ClassName, *DeprecationMessage);
		GraphNode->ReportError(EMessageSeverity::Warning, "Node should be replaced with new version if available");
	}

	// Ensure pin connections are valid and perform cleanup, if necessary.
	for (UEdGraphPin* Pin : GraphNode->Pins)
	{
		FName PinName = Pin->PinName;
		const UPinHandle* PinHandle = NodeAsset->GetPinByName(PinName, Pin->Direction);
		
		// Is the pin handle invalid?
		if (PinHandle == nullptr || !PinHandle->IsValidHandle())
		{
			Pin->BreakAllPinLinks(true);
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
	const bool bIsInputOrOutputClass = Class == UGameFlowNode_Input::StaticClass() || Class == UGameFlowNode_Output::StaticClass();
    // True if the given class does not belong to a category of classes which could not be directly instanced by the user.
	const bool bUserInstancedAllowed = bIsChildClass && !bIsInputOrOutputClass;

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
	// Start rebuilding the graph from an input node.
	ToRebuild.Enqueue(CurrentNode);
	
	// As long as there are nodes to build, keep iterating.
	while(!ToRebuild.IsEmpty())
	{
		// Pick the next node to rebuild.
		ToRebuild.Dequeue(CurrentNode);
		const UGameFlowNode* CurrentNodeAsset = CurrentNode->GetNodeAsset();
		CurrentNode->bIsRebuilding = true;
		
		for(UPinHandle* OutputPinHandle : CurrentNodeAsset->GetPinsByDirection(EGPD_Output))
		{
			const bool bIsValidHandle = IsValid(OutputPinHandle) && OutputPinHandle->IsValidHandle();
			// If the output pin is not valid, skip to the next iteration; it cannot be processed.
			ensureMsgf(bIsValidHandle,
				TEXT("Ensure condition failed: One of %s output pin handles is invalid; Pin rebuild was skipped."), *CurrentNodeAsset->GetName());
			if(!bIsValidHandle) continue;
			
			for(const auto& Pin : OutputPinHandle->GetConnections())
			{
				const UGameFlowNode* ConnectedNode = Pin->GetNodeOwner();
				const FName& ConnectedPinName = Pin->PinName;
				
				const bool bIsValidConnection = IsValid(ConnectedNode);
				const bool bIsValidPinName = !ConnectedPinName.IsEqual(EName::None);
				// If the next node is invalid or an input pin name is None, log a warning to the Unreal console and skip this iteration.
				ensureMsgf(bIsValidConnection, TEXT("Ensure condition failed: %s output pin of %s node is connected to an invalid node."),
					*OutputPinHandle->PinName.ToString(), *CurrentNodeAsset->GetName());
				ensureMsgf(bIsValidPinName, TEXT("Ensure condition failed: %s output pin of %s node is connected to a PinHandle with an invalid pin name: '%s'"),
					*OutputPinHandle->PinName.ToString(), *CurrentNodeAsset->GetName(), *ConnectedPinName.ToString());
				if(!bIsValidConnection || !bIsValidPinName) continue;
				
				// Get connected graph node.
				UGameFlowGraphNode* GraphNode = Graph.GetGraphNodeByAsset(ConnectedNode);
                GraphNode->bIsRebuilding = true;
				
				UEdGraphPin* FromPin = CurrentNode->FindPin(OutputPinHandle->PinName);
				UEdGraphPin* DestinationPin = GraphNode->FindPin(ConnectedPinName);
				
				// After finding the current node output pin and the next node input pin,
				// create a connection between the two.
				TryCreateConnection(FromPin, DestinationPin);
			
				// TODO | Need to implement some system to avoid multiple rebuild of the
				// TODO | same node. Right now it works with no issues because game flow
				// TODO | schema does not allow output pins to have more than one connection.

				GraphNode->bIsRebuilding = false;
				// Enqueue the next node, we'll need to rebuild it.
				ToRebuild.Enqueue(GraphNode);
			}
		}
		// Unmark the rebuild flag when the process has finished.
		CurrentNode->bIsRebuilding = false;
	}
}

void UGameFlowGraphSchema::RecreateNodeConnections(const UGameFlowGraph& Graph, UGameFlowGraphNode* GraphNode,
                                                   const TArray<EEdGraphPinDirection> Directions) const
{
	for (UEdGraphPin* Pin : GraphNode->Pins)
	{
		UE_LOG(LogGameFlow, Display, TEXT("Recreating connections for pin '%s' of '%s' node."),
			*Pin->PinName.ToString(), *GraphNode->GetNodeAsset()->GetName());
		if (!Directions.Contains(Pin->Direction)) continue;

		const UGameFlowNode* NodeAsset = GraphNode->GetNodeAsset();
		
		UPinHandle* PinHandle;
		// Find the node and pin to which the current node is connected to.
		if(Pin->Direction == EGPD_Output)
		{
			PinHandle = NodeAsset->Outputs.FindRef(Pin->PinName);
		}
		else
		{
			PinHandle = NodeAsset->Inputs.FindRef(Pin->PinName);
		}

		const bool bIsValidHandle = IsValid(PinHandle) && PinHandle->IsValidHandle();
		// If pin is not valid skip to next iteration, it cannot be processed.
		ensureMsgf(bIsValidHandle, TEXT("Ensure condition failed: '%s' pin handle of '%s' node is invalid; Pin rebuild was skipped."),
			*Pin->PinName.ToString(), *NodeAsset->GetName());
		if (PinHandle == nullptr || !PinHandle->IsValidHandle()) continue;
		
		for(const auto& ConnectedPinHandle : PinHandle->GetConnections())
		{
			const UGameFlowNode* ConnectedNode = ConnectedPinHandle->GetNodeOwner();
			const FName& InPinName = ConnectedPinHandle->PinName;

			const bool bIsValidNode = IsValid(ConnectedNode);
			const bool bIsValidPinName = !InPinName.IsEqual(EName::None);
			// If the next node is invalid or the pin name is None, skip this iteration.
			ensureMsgf(bIsValidNode, TEXT("Ensure condition failed: '%s' pin of '%s' node is connected to an invalid node."),
				*PinHandle->PinName.ToString(), *NodeAsset->GetName());
			ensureMsgf(bIsValidPinName, TEXT("Ensure condition failed: '%s' pin of '%s' node is connected to a PinHandle with an invalid pin name: '%s'"),
				*PinHandle->PinName.ToString(), *NodeAsset->GetName(), *InPinName.ToString());
			if (!bIsValidNode || !bIsValidPinName)
				continue;
			
			const UGameFlowGraphNode* ConnectedGraphNode = Graph.GetGraphNodeByAsset(ConnectedNode);
			// If the connected node asset does not have a graph representation, skip connections rebuild. 
			ensureMsgf(ConnectedGraphNode, TEXT("Ensure condition failed: node '%s' has no associated graph node. Graph connection rebuild skipped"),
				*ConnectedNode->GetName());
			if (!IsValid(ConnectedGraphNode)) continue;
			
			UEdGraphPin* OtherPin = ConnectedGraphNode->FindPin(InPinName);
			// After finding the current node output pin and the next node input pin,
			// create a connection between the two.
			TryCreateConnection(Pin, OtherPin);
		}
	}
}


