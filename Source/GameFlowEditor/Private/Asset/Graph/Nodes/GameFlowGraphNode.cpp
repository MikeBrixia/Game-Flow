// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "GameFlowEditor.h"
#include "GameFlowAsset.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Asset/Graph/Actions/FGameFlowSchemaAction_ReplaceNode.h"
#include "Asset/Graph/Nodes/FGameFlowGraphNodeCommands.h"
#include "Config/FGameFlowNodeInfo.h"
#include "Config/GameFlowEditorSettings.h"
#include "Widget/SGameFlowReplaceNodeDialog.h"
#include "Widget/Nodes/SGameFlowNode.h"

UGameFlowGraphNode::UGameFlowGraphNode()
{
	ContextMenuCommands = MakeShared<FUICommandList>();
}

void UGameFlowGraphNode::ConfigureContextMenuAction()
{
	const FGameFlowGraphNodeCommands& GraphNodeCommands = FGameFlowGraphNodeCommands::Get();
    
	// Configure core commands
	{
		ContextMenuCommands->MapAction(GraphNodeCommands.ReplaceNode,
								   FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::OnReplacementRequest),
								   FCanExecuteAction::CreateUObject(this, &UGameFlowGraphNode::CanBeReplaced),
								   FIsActionChecked::CreateUObject(this, &UGameFlowGraphNode::CanBeReplaced),
								   FIsActionButtonVisible::CreateUObject(this, &UGameFlowGraphNode::CanBeReplaced));
		
		ContextMenuCommands->MapAction(GraphNodeCommands.RemoveNode,
								   FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::DestroyNode),
								   FCanExecuteAction::CreateUObject(this, &UGameFlowGraphNode::CanUserDeleteNode),
								   FIsActionChecked::CreateUObject(this, &UGameFlowGraphNode::CanUserDeleteNode),
								   FIsActionButtonVisible::CreateUObject(this, &UGameFlowGraphNode::CanUserDeleteNode));
		
		ContextMenuCommands->MapAction(GraphNodeCommands.ValidateNode,
		                           FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::OnValidationRequest));
	}

	// Configure debug commands
	{
		ContextMenuCommands->MapAction(GraphNodeCommands.AddBreakpoint,
								   FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::OnAddBreakpointRequest),
								   FCanExecuteAction::CreateUObject(this, &UGameFlowGraphNode::CanAddBreakpoint),
								   FIsActionChecked::CreateUObject(this, &UGameFlowGraphNode::CanAddBreakpoint),
								   FIsActionButtonVisible::CreateUObject(this, &UGameFlowGraphNode::CanAddBreakpoint));
	
		ContextMenuCommands->MapAction(GraphNodeCommands.RemoveBreakpoint,
									   FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::OnRemoveBreakpointRequest),
									   FCanExecuteAction::CreateUObject(this, &UGameFlowGraphNode::CanRemoveBreakpoint),
									   FIsActionChecked::CreateUObject(this, &UGameFlowGraphNode::CanRemoveBreakpoint),
									   FIsActionButtonVisible::CreateUObject(this, &UGameFlowGraphNode::CanRemoveBreakpoint));
		
		ContextMenuCommands->MapAction(GraphNodeCommands.EnableBreakpoint,
									   FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::OnEnableBreakpointRequest),
									   FCanExecuteAction::CreateUObject(this, &UGameFlowGraphNode::CanEnableBreakpoint),
									   FIsActionChecked::CreateUObject(this, &UGameFlowGraphNode::CanEnableBreakpoint),
									   FIsActionButtonVisible::CreateUObject(this, &UGameFlowGraphNode::CanEnableBreakpoint));

		ContextMenuCommands->MapAction(GraphNodeCommands.DisableBreakpoint,
									   FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::OnDisableBreakpointRequest),
									   FCanExecuteAction::CreateUObject(this, &UGameFlowGraphNode::CanDisableBreakpoint),
									   FIsActionChecked::CreateUObject(this, &UGameFlowGraphNode::CanDisableBreakpoint),
									   FIsActionButtonVisible::CreateUObject(this, &UGameFlowGraphNode::CanDisableBreakpoint));
	}
}

void UGameFlowGraphNode::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();
	
	UGameFlowEditorSettings* Settings = UGameFlowEditorSettings::Get();
	// Get node asset info from config.
	Info = Settings->NodesTypes.FindChecked(NodeAsset->TypeName);

	// Listen to game flow assets events.
	NodeAsset->OnAssetRedirected.AddUObject(this, &UGameFlowGraphNode::OnLiveOrHotReloadCompile);
	
	// Listen to Unreal Editor blueprint compilation events.
	GEditor->OnBlueprintCompiled().AddUObject(this, &UGameFlowGraphNode::OnAssetCompiled);
	GEditor->OnBlueprintPreCompile().AddUObject(this, &UGameFlowGraphNode::OnAssetBlueprintPreCompiled);

	// Initialize node.
	AllocateDefaultPins();
	ConfigureContextMenuAction();
}

void UGameFlowGraphNode::OnReplacementRequest()
{
	const TSharedRef<SGameFlowReplaceNodeDialog> ReplaceNodeDialog = SNew(SGameFlowReplaceNodeDialog);
	const int32 PressedButtonIndex = ReplaceNodeDialog->ShowModal();
	UClass* PickedClass = ReplaceNodeDialog->GetPickedClass();
	
	// If user has picked a valid and different UCLASS and confirmed replacement, then replace the node.
	if(PressedButtonIndex == 0 && PickedClass != nullptr
		&& NodeAsset->GetClass() != PickedClass)
	{
		FGameFlowSchemaAction_ReplaceNode ReplaceNodeAction(this, PickedClass);
		ReplaceNodeAction.PerformAction(GetGraph(), nullptr, FVector2d::ZeroVector, true);
	}
}

void UGameFlowGraphNode::OnValidationRequest()
{
	const UGameFlowGraphSchema* Schema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	Schema->ValidateNodeAsset(this);
}

void UGameFlowGraphNode::OnAddBreakpointRequest()
{
	NodeAsset->bBreakpointEnabled = true;
	
	// TODO Implement debug features
}

void UGameFlowGraphNode::OnRemoveBreakpointRequest()
{
	NodeAsset->bBreakpointEnabled = false;
	
	// TODO Implement debug features
}

void UGameFlowGraphNode::OnDisableBreakpointRequest()
{
	NodeAsset->bBreakpointEnabled = false;
	// TODO Implement debug features
}

void UGameFlowGraphNode::OnEnableBreakpointRequest()
{
	NodeAsset->bBreakpointEnabled = true;
	// TODO Implement debug features
}

void UGameFlowGraphNode::OnPinRemoved(UEdGraphPin* InRemovedPin)
{
	// Break pin graph connections.
	BreakAllNodeLinks();
	
	// Remove logical connections.
	if(InRemovedPin->Direction == EGPD_Input)
	{
		NodeAsset->RemoveInputPin(InRemovedPin->PinName);
	}
	else if(InRemovedPin->Direction == EGPD_Output)
	{
		NodeAsset->RemoveOutputPin(InRemovedPin->PinName);
	}
}

void UGameFlowGraphNode::PinConnectionListChanged(UEdGraphPin* Pin)
{
	FScopedTransaction Transaction(NSLOCTEXT("GameFlowEditor", "Pin Connection List changed", "Rebuild pin connections"));
	
	Super::PinConnectionListChanged(Pin);

	Pin->Modify();
	NodeAsset->Modify();
	
	// Break this pin logical connection, we need to rebuild them.
	if(Pin->Direction == EGPD_Input)
	{
		NodeAsset->RemoveInputPort(Pin->PinName);
	}
	else if(Pin->Direction == EGPD_Output)
	{
		NodeAsset->RemoveOutputPort(Pin->PinName);
	}

	// Recreate logical connections between game flow nodes using graph pin data.
	for(UEdGraphPin* ConnectedPin : Pin->LinkedTo)
	{
		UGameFlowNode* ConnectedNodeAsset = CastChecked<UGameFlowNode>(ConnectedPin->DefaultObject);
		if(Pin->Direction == EGPD_Input)
		{
			NodeAsset->AddInputPort(Pin->PinName, {ConnectedPin->PinName, ConnectedNodeAsset});
		}
		else if(Pin->Direction == EGPD_Output)
		{
			NodeAsset->AddOutputPort(Pin->PinName, {ConnectedPin->PinName, ConnectedNodeAsset});
		}
	}
}

void UGameFlowGraphNode::DestroyNode()
{
	const UGameFlowGraphSchema* GameFlowSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	GameFlowSchema->BreakNodeLinks(*this);

	GetGraph()->RemoveNode(this, false);
}

void UGameFlowGraphNode::OnAssetSelected(const FAssetData& AssetData)
{
}

void UGameFlowGraphNode::OnAssetValidated()
{
	const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	GraphSchema->ValidateNodeAsset(this);
	// Notify listeners this node has been validated.
	OnValidationResult.Broadcast();
}

void UGameFlowGraphNode::OnLiveOrHotReloadCompile()
{
	// Mark as pending compilation on cpp compile(live coding or hot reload).
	bPendingCompilation = true;
	// Call default asset compilation callback.
	OnAssetCompiled();
}

void UGameFlowGraphNode::OnAssetCompiled()
{
	// Reconstruct node only if it is pending compile.
	if(bPendingCompilation)
	{
		const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
		// Ensure compiled asset is valid.
		GraphSchema->ValidateNodeAsset(this);
		
		// reconstruct the compiled asset with the updated properties/logic.
		ReconstructNode();
		
		// Notify listeners this node has been compiled.
		OnNodeAssetChanged.Broadcast();

		// Node has finished compilation, remove the mark from it.
		bPendingCompilation = false;
	}
}

void UGameFlowGraphNode::OnAssetBlueprintPreCompiled(UBlueprint* Blueprint)
{
	// If the compiled node is the graph encapsulated node, mark it as a pending compile graph node.
	bPendingCompilation = Blueprint != nullptr && Blueprint == NodeAsset->GetClass()->ClassGeneratedBy
	                      && GetGraph()->Nodes.Contains(this);
}

void UGameFlowGraphNode::AllocateDefaultPins()
{
	// Read input pins names from node asset and create graph pins.
	for(const FName& PinName : NodeAsset->GetInputPins())
	{
		CreateNodePin(EGPD_Input, PinName, false);
	}

	// Read output pins names from node asset and create graph pins.
	for(const FName& PinName : NodeAsset->GetOutputPins())
	{
		CreateNodePin(EGPD_Output, PinName, false);
	}
}

void UGameFlowGraphNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);
	
	const FGameFlowGraphNodeCommands& GraphNodeCommands = FGameFlowGraphNodeCommands::Get();
	
	// When only the node is selected, show available context actions.
	if(Context->Pin != nullptr)
	{
		// Pin handle actions
		{
			// TODO Should implement pin handle context actions.
		}
		
		// Pin debug actions
		{
			// TODO Should implement pin debug actions.
		}
	}
	else if(Context->Node != nullptr)
	{
		// Generic node actions.
		{
			FToolMenuSection& GameFlowSection = Menu->AddSection("GameFlow", NSLOCTEXT("FGameFlowNode", "NodeContextAction", "Node actions"));
			GameFlowSection.AddMenuEntryWithCommandList(GraphNodeCommands.ValidateNode, ContextMenuCommands);
			GameFlowSection.AddMenuEntryWithCommandList(GraphNodeCommands.ReplaceNode, ContextMenuCommands);
			GameFlowSection.AddMenuEntryWithCommandList(GraphNodeCommands.RemoveNode, ContextMenuCommands);
		}

		// Debug actions
		{
			FToolMenuSection& DebugSection = Menu->AddSection("DebugSection", NSLOCTEXT("FGameFlowNode", "NodeDebugContextAction", "Debug"));
			DebugSection.AddMenuEntryWithCommandList(GraphNodeCommands.AddBreakpoint, ContextMenuCommands);
			DebugSection.AddMenuEntryWithCommandList(GraphNodeCommands.RemoveBreakpoint, ContextMenuCommands);
			DebugSection.AddMenuEntryWithCommandList(GraphNodeCommands.EnableBreakpoint, ContextMenuCommands);
			DebugSection.AddMenuEntryWithCommandList(GraphNodeCommands.DisableBreakpoint, ContextMenuCommands);
		}
	} 
}

FName UGameFlowGraphNode::CreateUniquePinName(FName SourcePinName) const
{
	FString GeneratedName = SourcePinName.ToString();
	int Number;
	// Handle numeric-only pin name generation.
	if(GeneratedName.IsNumeric())
	{
		Number = FCString::Atoi(*GeneratedName);
		GeneratedName = FString::FromInt(Number + 1);
	}
	// Handle text pin name generation.
	else
	{
		Number = GetNum(GeneratedName);
		GeneratedName = FString::Printf(TEXT("NewPin_%d"), Number + 1);
	}

	return FName(GeneratedName);
}

TSharedPtr<SGraphNode> UGameFlowGraphNode::CreateVisualWidget()
{
	// Use UCLASS display name attribute value as node title.
	const FText TitleText = GetNodeTitle(ENodeTitleType::EditableTitle);
	
	TSharedRef<SGameFlowNode> NodeWidget = SNew(SGameFlowNode)
		                                   .Node(this)
		                                   .TitleText(TitleText);
	return NodeWidget;
}

FText UGameFlowGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	const bool bIsInputOrOutputNode = NodeAsset->IsA(UGameFlowNode_Input::StaticClass()) ||
		                              NodeAsset->IsA(UGameFlowNode_Output::StaticClass());
	return bIsInputOrOutputNode? FText::FromString(NodeAsset->GetName()) : NodeAsset->GetClass()->GetDisplayNameText();
}

bool UGameFlowGraphNode::IsOrphan() const
{
	// Find all input pins which have a connection.
	const TArray<UEdGraphPin*> ConnectedInputPins = Pins.FilterByPredicate([] (const UEdGraphPin* Pin)
	{
		return Pin->Direction == EGPD_Input && Pin->HasAnyConnections();
	});

	// If none of the input pins have at least
	// one connection, this node is orphan.
	return ConnectedInputPins.Num() == 0;
}

bool UGameFlowGraphNode::CanUserDeleteNode() const
{
	const FText NodeDisplayName = NodeAsset->GetClass()->GetDisplayNameText();
	// User will be able to delete all types of nodes except 'Start' and 'Finish'
	return !(NodeDisplayName.EqualTo(INVTEXT("Start")) || NodeDisplayName.EqualTo(INVTEXT("Finish")));
}

bool UGameFlowGraphNode::CanBeReplaced() const
{
	return !NodeAsset->IsA(UGameFlowNode_Input::StaticClass())
	       && !NodeAsset->IsA(UGameFlowNode_Output::StaticClass());
}

void UGameFlowGraphNode::ReconstructNode()
{
	const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	
	const UGameFlowEditorSettings* GameFlowEditorSettings = UGameFlowEditorSettings::Get();
	Info = GameFlowEditorSettings->NodesTypes.FindRef(NodeAsset->TypeName);
	
	BreakAllNodeLinks();
	Pins.Empty();
	AllocateDefaultPins();
	
	const UGameFlowGraph& GameFlowGraph = *CastChecked<UGameFlowGraph>(GetGraph());
	// Recompile node and recreate it's node connections.
	GraphSchema->RecreateNodeConnections(GameFlowGraph, this, TArray { EGPD_Input, EGPD_Output });
}

void UGameFlowGraphNode::ReportError(EMessageSeverity::Type MessageSeverity)
{
	bHasCompilerMessage = MessageSeverity == EMessageSeverity::Error ||
		    MessageSeverity == EMessageSeverity::Warning ||
		    MessageSeverity == EMessageSeverity::Info ||
			MessageSeverity == EMessageSeverity::PerformanceWarning;
	ErrorType = MessageSeverity;
}

void UGameFlowGraphNode::SetNodeAsset(UGameFlowNode* Node)
{
	NodeAsset = Node;
	// Read new info data from config using the new node asset type.
	UGameFlowEditorSettings* Settings = UGameFlowEditorSettings::Get();
	Info = Settings->NodesTypes.FindChecked(NodeAsset->TypeName);
	
	// Notify listeners that the node asset has been changed.
	if(OnNodeAssetChanged.IsBound())
	{
		OnNodeAssetChanged.Broadcast();
	}
}

void UGameFlowGraphNode::OnRenameNode(const FString& NewName)
{
	Super::OnRenameNode(NewName);
	
	const FName NewNodeName (NewName);
	UGameFlowAsset* GameFlowAsset = NodeAsset->GetTypedOuter<UGameFlowAsset>();
	// Rename node object only if the supplied name is unique.
	if(IsUniqueObjectName(NewNodeName, GameFlowAsset)
		&& GetCanRenameNode())
	{
		if(UGameFlowNode_Input* InputNode = Cast<UGameFlowNode_Input>(NodeAsset))
		{
			GameFlowAsset->CustomInputs.Remove(NodeAsset->GetFName());
			GameFlowAsset->CustomInputs.Add(NewNodeName, InputNode);
		}
		else if(UGameFlowNode_Output* OutputNode = Cast<UGameFlowNode_Output>(NodeAsset))
		{
			GameFlowAsset->CustomOutputs.Remove(NodeAsset->GetFName());
			GameFlowAsset->CustomOutputs.Add(NewNodeName, OutputNode);
		}
		NodeAsset->Rename(*NewName);
	}
	else
	{
		// Notify the user there has been an error with node renaming.
		UE_LOG(LogGameFlow, Error, TEXT("Object name '%s' is already in use inside '%s' asset!"), *NewName, *GameFlowAsset->GetName())
	}
}

UEdGraphPin* UGameFlowGraphNode::CreateNodePin(const EEdGraphPinDirection PinDirection, FName PinName, bool bAddToAsset)
{
	if(bAddToAsset)
	{
		switch(PinDirection)
		{
			// Direction is not valid, do nothing.
			default: break;
			
			// Add input pin to node asset.
			case EGPD_Input:
				{
					TArray<FName> InputPins = NodeAsset->GetInputPins();
					const FName PreviousName = InputPins.Num() > 0 ? InputPins.Last() : "None";
					PinName = CreateUniquePinName(PreviousName);
					NodeAsset->AddInputPin(PinName, {});
					break;
				}
			// Add output pin to node asset.	
			case EGPD_Output:
				{
					TArray<FName> OutputPins = NodeAsset->GetOutputPins();
					const FName PreviousName = OutputPins.Num() > 0 ? OutputPins.Last() : "None";
					PinName = CreateUniquePinName(PreviousName);
					NodeAsset->AddOutputPin(PinName, {});
					break;
				}
		}
	}
	
	UEdGraphPin* Pin = nullptr;
	// Create pin object only if name is valid.
	if(!PinName.IsEqual(EName::None))
	{
		const FEdGraphPinType PinType = GetGraphPinType();
		// Create the pin object.
		Pin = CreatePin(PinDirection, PinType, PinName);
		Pin->PinFriendlyName = FText::FromName(PinName);
		Pin->DefaultObject = NodeAsset;
	}
	return Pin;
}


