// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "GameFlowEditor.h"
#include "GameFlowAsset.h"
#include "LevelEditor.h"
#include "Asset/GameFlowAssetToolkit.h"
#include "Asset/GameFlowEditorStyleWidgetStyle.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Asset/Graph/Actions/FGameFlowSchemaAction_ReplaceNode.h"
#include "Asset/Graph/Nodes/FGameFlowGraphNodeCommands.h"
#include "Config/FGameFlowNodeInfo.h"
#include "Config/GameFlowEditorSettings.h"
#include "Widget/SGameFlowReplaceNodeDialog.h"
#include "Widget/Nodes/SGameFlowNode.h"

#define LOCTEXT_NAMESPACE "FGameFlowEditor"

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

void UGameFlowGraphNode::SetNodeInfo(FGameFlowNodeInfo NewInfo)
{
	this->Info = NewInfo; 
}

FText UGameFlowGraphNode::GetTooltipText() const
{
	return NodeAsset->GetClass()->GetToolTipText();
}

FSlateIcon UGameFlowGraphNode::GetIconAndTint(FLinearColor& OutColor) const
{
	FString StyleKey;
	NodeAsset->GetNodeIconInfo(StyleKey, OutColor);
	const FSlateIcon NodeIcon { FGameFlowEditorStyle::TypeName,FName(StyleKey) };
	return NodeIcon;
}

FGameFlowNodeInfo& UGameFlowGraphNode::GetNodeInfo()
{
	return Info;
}

UGameFlowNode* UGameFlowGraphNode::GetNodeAsset() const
{
	return NodeAsset; 
}

void UGameFlowGraphNode::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();
	
	UGameFlowEditorSettings* Settings = UGameFlowEditorSettings::Get();
	// Get node asset info from config.
	Info = Settings->NodesTypes.FindChecked(NodeAsset->TypeName);

	// Listen to game flow asset events.
	NodeAsset->OnAssetRedirected.AddUObject(this, &UGameFlowGraphNode::OnLiveOrHotReloadCompile);
	NodeAsset->OnErrorEvent.AddUObject(this, &UGameFlowGraphNode::ReportError);
	NodeAsset->OnAssetExecuted.AddDynamic(this, &UGameFlowGraphNode::OnNodeAssetExecuted);
	
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

bool UGameFlowGraphNode::CanAddBreakpoint() const
{
	return !NodeAsset->bBreakpointEnabled; 
}

bool UGameFlowGraphNode::CanRemoveBreakpoint() const
{
	return NodeAsset->bBreakpointEnabled; 
}

bool UGameFlowGraphNode::CanEnableBreakpoint() const
{
	return !NodeAsset->bBreakpointEnabled; 
}

bool UGameFlowGraphNode::CanDisableBreakpoint() const
{
	return NodeAsset->bBreakpointEnabled; 
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
	Super::PinConnectionListChanged(Pin);
	
	FScopedTransaction Transaction(NSLOCTEXT("GameFlowEditor", "Pin Connection List changed", "Rebuild pin connections"));
	Pin->Modify();
	NodeAsset->Modify();
	
	// We don't want to touch logical pin handles during graph node rebuild process,
	// it could lead do data corruption.
	if(!bIsRebuilding)
	{
		UPinHandle* PinHandle = NodeAsset->GetPinByName(Pin->PinName, Pin->Direction);
		PinHandle->CutAllConnections();
		// Recreate logical connections between game flow nodes using graph pin data.
		for(UEdGraphPin* ConnectedPin : Pin->LinkedTo)
		{
			ConnectedPin->Modify();
			UGameFlowNode* ConnectedNodeAsset = CastChecked<UGameFlowNode>(ConnectedPin->DefaultObject);
			ConnectedNodeAsset->Modify();
			
			UPinHandle* ConnectedPinHandle = ConnectedNodeAsset->GetPinByName(ConnectedPin->PinName, ConnectedPin->Direction);
			PinHandle->CreateConnection(ConnectedPinHandle);
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

void UGameFlowGraphNode::OnNodeAssetPinTriggered(UPinHandle* PinHandle)
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
	for(const FName& PinName : NodeAsset->GetInputPinsNames())
	{
		CreateNodePin(EGPD_Input, PinName, false);
	}

	// Read output pins names from node asset and create graph pins.
	for(const FName& PinName : NodeAsset->GetOutputPinsNames())
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

FEdGraphPinType UGameFlowGraphNode::GetGraphPinType() const
{
	FEdGraphPinType OutputPinInfo = {};
	OutputPinInfo.PinCategory = UEdGraphSchema_K2::PC_Exec;
	return OutputPinInfo;
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

FLinearColor UGameFlowGraphNode::GetNodeTitleColor() const
{
	return Info.TitleBarColor; 
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

bool UGameFlowGraphNode::GetCanRenameNode() const
{
	const bool bIsInputOrOutputNode = UGameFlowNode_Input::StaticClass() || UGameFlowNode_Output::StaticClass();
		
	const FString NodeName = NodeAsset->GetName();
	const bool bIsDefaultInputOrOutput = NodeName.Equals("Start") || NodeName.Equals("Finish");
	return bIsInputOrOutputNode && !bIsDefaultInputOrOutput;
}

bool UGameFlowGraphNode::ShowPaletteIconOnNode() const
{
	return true;
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

bool UGameFlowGraphNode::Modify(bool bAlwaysMarkDirty)
{
	if(NodeAsset != nullptr) NodeAsset->Modify();
	return Super::Modify(bAlwaysMarkDirty);
}

void UGameFlowGraphNode::OnNodeAssetExecuted(UInputPinHandle* InputPinHandle)
{
	bIsActive = true;
	
	if(NodeAsset->bBreakpointEnabled || (InputPinHandle != nullptr && InputPinHandle->bIsBreakpointEnabled))
	{
		// Pause the play session when the breakpoint gets hit.
		GEditor->SetPIEWorldsPaused(true);

		UEdGraphPin* GraphPin = FindPin(InputPinHandle->PinName);
		
        UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(GetGraph());
		// Notify graph of the breakpoint hit.
		GameFlowGraph->OnBreakpointHit(this, GraphPin);
	}
}

bool UGameFlowGraphNode::IsActiveNode() const
{
	return bIsActive;
}

bool UGameFlowGraphNode::IsRoot() const
{
	return NodeAsset->IsA(UGameFlowNode_Input::StaticClass()); 
}

bool UGameFlowGraphNode::IsLeaf() const
{
	bool bIsLeaf = true;
	TArray<UEdGraphPin*> OutputPins = Pins.FilterByPredicate([=](UEdGraphPin* Pin)
	{
		return Pin->Direction == EGPD_Output;
	});
	
	for(const UEdGraphPin* Pin : OutputPins)
	{
		// If at least one pin has a connection,
		// this node is not a leaf.
		if(Pin->HasAnyConnections())
		{
			bIsLeaf = false;
			break;
		}
	}

	return bIsLeaf;
}

void UGameFlowGraphNode::ReportError(EMessageSeverity::Type MessageSeverity, FString ErrorMessage)
{
	bHasCompilerMessage = MessageSeverity == EMessageSeverity::Error ||
		    MessageSeverity == EMessageSeverity::Warning ||
		    MessageSeverity == EMessageSeverity::Info ||
			MessageSeverity == EMessageSeverity::PerformanceWarning;
	ErrorType = MessageSeverity;
	ErrorMsg = ErrorMessage;
}

void UGameFlowGraphNode::SetNodeAsset(UGameFlowNode* Node)
{
	NodeAsset = Node;
	if(Node != nullptr)
	{
		// Read new info data from config using the new node asset type.
		UGameFlowEditorSettings* Settings = UGameFlowEditorSettings::Get();
		Info = Settings->NodesTypes.FindChecked(NodeAsset->TypeName);
	
		// Notify listeners that the node asset has been changed.
		if(OnNodeAssetChanged.IsBound())
		{
			OnNodeAssetChanged.Broadcast();
		}
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
					TArray<FName> InputPins = NodeAsset->GetInputPinsNames();
					const FName PreviousName = InputPins.Num() > 0 ? InputPins.Last() : "None";
					PinName = CreateUniquePinName(PreviousName);
					NodeAsset->AddInputPin(PinName);
					break;
				}
			
			// Add output pin to node asset.	
			case EGPD_Output:
				{
					TArray<FName> OutputPins = NodeAsset->GetOutputPinsNames();
					const FName PreviousName = OutputPins.Num() > 0 ? OutputPins.Last() : "None";
					PinName = CreateUniquePinName(PreviousName);
					NodeAsset->AddOutputPin(PinName);
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

#undef LOCTEXT_NAMESPACE