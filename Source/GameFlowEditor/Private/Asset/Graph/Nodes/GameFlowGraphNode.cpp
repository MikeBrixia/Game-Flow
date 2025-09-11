// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "GameFlowEditor.h"
#include "GameFlowAsset.h"
#include "GraphEditorActions.h"

#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 4) || ENGINE_MAJOR_VERSION >= 6

#include "EdGraphSchema_K2.h"
#include "ScopedTransaction.h"
#include "ToolMenu.h"

#endif

#include "AssetViewUtils.h"
#include "Asset/GameFlowEditorStyleWidgetStyle.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Asset/Graph/Actions/FGameFlowSchemaAction_ReplaceNode.h"
#include "Asset/Graph/Nodes/FGameFlowGraphNodeCommands.h"
#include "Config/FGameFlowNodeInfo.h"
#include "Config/GameFlowEditorSettings.h"
#include "Engine/StreamableManager.h"
#include "Framework/Commands/GenericCommands.h"
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
	const FGraphEditorCommandsImpl& GraphEditorCommands = FGraphEditorCommands::Get();
	const FGenericCommands& GenericCommands = FGenericCommands::Get();
	
	// Configure core commands
	{
		ContextMenuCommands->MapAction(GenericCommands.Copy,
			                           FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::PrepareForCopying));
		
		ContextMenuCommands->MapAction(GenericCommands.Paste,
									   FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::PostPasteNode));
		
		ContextMenuCommands->MapAction(GraphNodeCommands.ReplaceNode,
								   FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::OnReplacementRequest),
								   FCanExecuteAction::CreateUObject(this, &UGameFlowGraphNode::CanBeReplaced),
								   FIsActionChecked::CreateUObject(this, &UGameFlowGraphNode::CanBeReplaced),
								   FIsActionButtonVisible::CreateUObject(this, &UGameFlowGraphNode::CanBeReplaced));
		
		ContextMenuCommands->MapAction(GenericCommands.Delete,
								   FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::DestroyNode),
								   FCanExecuteAction::CreateUObject(this, &UGameFlowGraphNode::CanUserDeleteNode),
								   FIsActionChecked::CreateUObject(this, &UGameFlowGraphNode::CanUserDeleteNode),
								   FIsActionButtonVisible::CreateUObject(this, &UGameFlowGraphNode::CanUserDeleteNode));
		
		ContextMenuCommands->MapAction(GraphNodeCommands.ValidateNode,
		                           FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::OnValidationRequest));

		ContextMenuCommands->MapAction(GraphEditorCommands.ReconstructNodes,
			                       FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::ReconstructNode));
	}

	// Configure debug commands
	{
		ContextMenuCommands->MapAction(GraphEditorCommands.AddBreakpoint,
								   FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::OnAddBreakpointRequest),
								   FCanExecuteAction::CreateUObject(this, &UGameFlowGraphNode::CanAddBreakpoint),
								   FIsActionChecked::CreateUObject(this, &UGameFlowGraphNode::CanAddBreakpoint),
								   FIsActionButtonVisible::CreateUObject(this, &UGameFlowGraphNode::CanAddBreakpoint));
	
		ContextMenuCommands->MapAction(GraphEditorCommands.RemoveBreakpoint,
									   FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::OnRemoveBreakpointRequest),
									   FCanExecuteAction::CreateUObject(this, &UGameFlowGraphNode::CanRemoveBreakpoint),
									   FIsActionChecked::CreateUObject(this, &UGameFlowGraphNode::CanRemoveBreakpoint),
									   FIsActionButtonVisible::CreateUObject(this, &UGameFlowGraphNode::CanRemoveBreakpoint));
		
		ContextMenuCommands->MapAction(GraphEditorCommands.EnableBreakpoint,
									   FExecuteAction::CreateUObject(this, &UGameFlowGraphNode::OnEnableBreakpointRequest),
									   FCanExecuteAction::CreateUObject(this, &UGameFlowGraphNode::CanEnableBreakpoint),
									   FIsActionChecked::CreateUObject(this, &UGameFlowGraphNode::CanEnableBreakpoint),
									   FIsActionButtonVisible::CreateUObject(this, &UGameFlowGraphNode::CanEnableBreakpoint));

		ContextMenuCommands->MapAction(GraphEditorCommands.DisableBreakpoint,
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

void UGameFlowGraphNode::OnCommentTextCommitted(const FText& NewText, const ETextCommit::Type CommitType)
{
	OnUpdateCommentText(NewText.ToString());
}

FSlateIcon UGameFlowGraphNode::GetIconAndTint(FLinearColor& OutColor) const
{
	FString StyleKey;
	NodeAsset->GetNodeIconInfo(StyleKey, OutColor);
	const FSlateIcon NodeIcon { FGameFlowEditorStyle::TypeName,FName(StyleKey) };
	return NodeIcon;
}

void UGameFlowGraphNode::SetDebugEnabled(bool bEnabled)
{
	bDebugEnabled = bEnabled;
}

bool UGameFlowGraphNode::IsDebugEnabled() const
{
	return bDebugEnabled || NodeAsset->bForceDebugView;
}

FText UGameFlowGraphNode::GetDebugInfoText() const
{
	FString DebugInfoStatus;

	UGameFlowNode* InspectedNode = GetInspectedNodeInstance();
	if(InspectedNode == nullptr) return FText::FromString(DebugInfoStatus);
	
	// Automated UPROPERTY debug info generation.
	for (TFieldIterator<FProperty> PropIt(InspectedNode->GetClass()); PropIt; ++PropIt)
	{
		const FProperty* Property = *PropIt;
		if(Property->HasMetaData("GF_Debuggable") && Property->GetMetaData("GF_Debuggable") == "enabled")
		{
			FString CPP_PropertyName = *Property->GetNameCPP();
			FString PropertyType = Property->GetCPPType();
			FString PropertyValue;
			
			const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(InspectedNode);
			Property->ExportTextItem_Direct(PropertyValue, ValuePtr, nullptr, nullptr, 0);
			
			FString PropertyInfoStatus = FString::Printf(TEXT("%s %s: %s \n"),*PropertyType, *CPP_PropertyName, *PropertyValue); 
			DebugInfoStatus.Append(PropertyInfoStatus);
		}
	}
	
	// String used to display more advanced debug messages.
	FString CustomDebugString = InspectedNode->GetCustomDebugInfo();
	DebugInfoStatus.Append(CustomDebugString);
	
	return FText::FromString(DebugInfoStatus);
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

	// Read the node comment and choose whether we
	// should show the comment bubble or not.
	NodeComment = NodeAsset->SavedNodeComment;
	bool bVisible = IsCommentBubbleVisible();
	SetMakeCommentBubbleVisible(bVisible);
	
	// Initialize node.
	Initialize();
	ConfigureContextMenuAction();
}

void UGameFlowGraphNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin != nullptr && !FromPin->IsPendingKill())
	{
		const UGameFlowGraphSchema* GameFlowGraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
		GameFlowGraphSchema->ConnectToDefaultPin(FromPin, this);
	}
}

bool UGameFlowGraphNode::CanDuplicateNode() const
{
	return !(GEditor->IsPlayingSessionInEditor() || GEditor->IsPlayingWithOnlinePIE()
	       || GEditor->IsSimulatingInEditor() || GEditor->IsPlayingViaLauncher());
}

void UGameFlowGraphNode::PostPasteNode()
{
	Super::PostPasteNode();
	
	// Force pins handles connections to match copy-paste graph node connections.
	for (UEdGraphPin* Pin : Pins)
	{
		PinConnectionListChanged(Pin);
	}
}

void UGameFlowGraphNode::PrepareForCopying()
{
	Super::PrepareForCopying();
	bIsBeingCopyPasted = true;
}

void UGameFlowGraphNode::PostEditImport()
{
	Super::PostEditImport();
	
	const UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(GetGraph());
	UGameFlowNode* DuplicatedNodeAsset = DuplicateObject<UGameFlowNode>(NodeAsset, GameFlowGraph->GameFlowAsset);
	SetNodeAsset(DuplicatedNodeAsset);

    // New pins default object should be the duplicated node asset.
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin)
		{
			Pin->Modify();
			Pin->DefaultObject = DuplicatedNodeAsset;
		}
	}
	
	// Initialize pasted node.
	Initialize();
	ConfigureContextMenuAction();
}

void UGameFlowGraphNode::OnReplacementRequest()
{
	const TSharedRef<SGameFlowReplaceNodeDialog> ReplaceNodeDialog = SNew(SGameFlowReplaceNodeDialog);
	const int32 PressedButtonIndex = ReplaceNodeDialog->ShowModal();
	UClass* PickedClass = ReplaceNodeDialog->GetPickedClass();
	
	// If the user has picked a valid and different UCLASS and confirmed replacement, then replace the node.
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
	NodeAsset->bBreakpointPlaced = true;
	NodeAsset->bBreakpointEnabled = true;
}

void UGameFlowGraphNode::OnRemoveBreakpointRequest()
{
	NodeAsset->bBreakpointPlaced = false;
	NodeAsset->bBreakpointEnabled = false;
}

void UGameFlowGraphNode::OnDisableBreakpointRequest()
{
	NodeAsset->bBreakpointEnabled = false;
}

void UGameFlowGraphNode::OnEnableBreakpointRequest()
{
	NodeAsset->bBreakpointEnabled = true;
}

bool UGameFlowGraphNode::CanAddBreakpoint() const
{
	return !NodeAsset->bBreakpointPlaced; 
}

bool UGameFlowGraphNode::CanRemoveBreakpoint() const
{
	return NodeAsset->bBreakpointPlaced; 
}

bool UGameFlowGraphNode::CanEnableBreakpoint() const
{
	return !NodeAsset->bBreakpointEnabled && NodeAsset->bBreakpointPlaced; 
}

bool UGameFlowGraphNode::CanDisableBreakpoint() const
{
	return NodeAsset->bBreakpointEnabled && NodeAsset->bBreakpointPlaced; 
}

void UGameFlowGraphNode::OnAddPinBreakpointRequest(const UEdGraphPin* GraphPin)
{
	UPinHandle* PinHandle = NodeAsset->GetPinByName(GraphPin->PinName, GraphPin->Direction);
	PinHandle->bIsBreakpointPlaced = true;
	PinHandle->bIsBreakpointEnabled = true;
}

void UGameFlowGraphNode::OnRemovePinBreakpointRequest(const UEdGraphPin* GraphPin)
{
	UPinHandle* PinHandle = NodeAsset->GetPinByName(GraphPin->PinName, GraphPin->Direction);
	PinHandle->bIsBreakpointPlaced = false;
	PinHandle->bIsBreakpointEnabled = false;
}

void UGameFlowGraphNode::OnEnablePinBreakpointRequest(const UEdGraphPin* GraphPin)
{
	UPinHandle* PinHandle = NodeAsset->GetPinByName(GraphPin->PinName, GraphPin->Direction);
	PinHandle->bIsBreakpointEnabled = true;
}

void UGameFlowGraphNode::OnDisablePinBreakpointRequest(const UEdGraphPin* GraphPin)
{
	UPinHandle* PinHandle = NodeAsset->GetPinByName(GraphPin->PinName, GraphPin->Direction);
	PinHandle->bIsBreakpointEnabled = false;
}

bool UGameFlowGraphNode::CanAddPinBreakpoint(const UEdGraphPin* GraphPin) const
{
	UPinHandle* PinHandle = NodeAsset->GetPinByName(GraphPin->PinName, GraphPin->Direction);
	return !PinHandle->bIsBreakpointPlaced;
}

bool UGameFlowGraphNode::CanRemovePinBreakpoint(const UEdGraphPin* GraphPin) const
{
	UPinHandle* PinHandle = NodeAsset->GetPinByName(GraphPin->PinName, GraphPin->Direction);
	return PinHandle->bIsBreakpointPlaced;
}

bool UGameFlowGraphNode::CanEnablePinBreakpoint(const UEdGraphPin* GraphPin) const
{
	UPinHandle* PinHandle = NodeAsset->GetPinByName(GraphPin->PinName, GraphPin->Direction);
	return !PinHandle->bIsBreakpointEnabled && PinHandle->bIsBreakpointPlaced;
}

bool UGameFlowGraphNode::CanDisablePinBreakpoint(const UEdGraphPin* GraphPin) const
{
	UPinHandle* PinHandle = NodeAsset->GetPinByName(GraphPin->PinName, GraphPin->Direction);
	return PinHandle->bIsBreakpointEnabled && PinHandle->bIsBreakpointPlaced;
}

void UGameFlowGraphNode::TriggerBreakpoint(UPinHandle* PinHandle)
{
	// Breakpoint should only be triggered in debug mode.
	if (!IsDebugEnabled()) return;
	
	const bool bIsValidPin = PinHandle != nullptr;
	// The graph pin on which the breakpoint has been triggered. Can be nullptr
	UEdGraphPin* GraphPin = bIsValidPin ? FindPin(PinHandle->PinName) : nullptr;
	// Do we have an active breakpoint on this node OR on the triggered pin (if valid)? 
	if(NodeAsset->bBreakpointPlaced || (bIsValidPin && PinHandle->bIsBreakpointEnabled))
	{
		UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(GetGraph());
		// Notify the graph of the breakpoint hit.
		GameFlowGraph->OnBreakpointHit(this, GraphPin);
	}
}

void UGameFlowGraphNode::OnPinRemoved(UEdGraphPin* InRemovedPin)
{
	const bool bCanEditAsset = CanEditNodeAsset();
	// Break pin graph connections. if the asset can be edited,
	// notify all connected nodes.
	InRemovedPin->BreakAllPinLinks(bCanEditAsset);

	// If the node is reconstructing, rebuilding or being compiled; we
	// don't want to touch the node asset.
	if (bCanEditAsset)
	{
		NodeAsset->RemovePin(InRemovedPin->PinName, InRemovedPin->Direction);
	}
}

void UGameFlowGraphNode::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);
	
	FScopedTransaction Transaction(NSLOCTEXT("GameFlowEditor", "Pin Connection List changed", "Rebuild pin connections"));
	Pin->Modify();
	NodeAsset->Modify();
	
	// Are we allowed to edit the node asset?
	if(CanEditNodeAsset())
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

void UGameFlowGraphNode::OnAssetValidated()
{
	const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	GraphSchema->ValidateNodeAsset(this);
	// Notify listeners this node has been validated to trigger redraw.
	GetGraph()->NotifyGraphChanged();
}

void UGameFlowGraphNode::OnAssetCompiled()
{
	// Reconstruct the node only if it is pending compile.
	if(bPendingCompilation)
	{
		const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
		// Ensure the compiled asset is valid.
		GraphSchema->ValidateNodeAsset(this);
		
		// reconstruct the compiled asset with the updated properties/logic.
		ReconstructNode();
		
		// Node has finished compilation, remove the mark from it.
		bPendingCompilation = false;
	}
}

void UGameFlowGraphNode::MarkNodeAsPendingCompilation()
{
	bPendingCompilation = true;
}

void UGameFlowGraphNode::AllocateDefaultPins()
{
	// Read input pins names from the node asset and create graph pins.
	for (const FName& PinName : NodeAsset->GetInputPinsNames())
	{
		CreateNodePin(EGPD_Input, PinName, false);
	}

	// Read output pins names from the node asset and create graph pins.
	for (const FName& PinName : NodeAsset->GetOutputPinsNames())
	{
		CreateNodePin(EGPD_Output, PinName, false);
	}
}

void UGameFlowGraphNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	const FGameFlowGraphNodeCommands& GraphNodeCommands = FGameFlowGraphNodeCommands::Get();
	const FGraphEditorCommandsImpl& GraphEditorCommands = FGraphEditorCommands::Get();
	const FGenericCommands& GenericCommands = FGenericCommands::Get();

	Super::GetNodeContextMenuActions(Menu, Context);

	const UEdGraphPin* ContextPin = Context->Pin;
	UGameFlowGraphNode* const MutableThis = const_cast<UGameFlowGraphNode*>(this);
	// When only the node is selected, show available context actions.
	if (ContextPin != nullptr)
	{
		// Pin handle actions
		{
		}

		// Pin debug actions
		{
			FToolMenuSection& PinDebugSection = Menu->AddSection(
				"GameFlow", NSLOCTEXT("FGameFlowNode", "PinContextAction", "Debug actions"));

			// Add Breakpoint (pin)
			PinDebugSection.AddMenuEntry(
				"GF_AddBreakpointOnPin",
				LOCTEXT("GF_AddBreakpointOnPin_Label", "Add Breakpoint"),
				LOCTEXT("GF_AddBreakpointOnPin_Tooltip", "Add a breakpoint on this pin"),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateLambda([MutableThis, ContextPin]()
					{
						MutableThis->OnAddPinBreakpointRequest(ContextPin);
					}),
					FCanExecuteAction::CreateLambda([MutableThis, ContextPin]()
					{
						return MutableThis->CanAddPinBreakpoint(ContextPin);
					}),
					FGetActionCheckState(),
					FIsActionButtonVisible::CreateLambda([MutableThis, ContextPin]()
					{
						return MutableThis->CanAddPinBreakpoint(ContextPin);
					})
				)
			);

			// Remove Breakpoint (pin)
			PinDebugSection.AddMenuEntry(
				"GF_RemoveBreakpointOnPin",
				LOCTEXT("GF_RemoveBreakpointOnPin_Label", "Remove Breakpoint"),
				LOCTEXT("GF_RemoveBreakpointOnPin_Tooltip", "Remove a breakpoint from this pin"),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateLambda([MutableThis, ContextPin]()
					{
						MutableThis->OnRemovePinBreakpointRequest(ContextPin);
					}),
					FCanExecuteAction::CreateLambda([MutableThis, ContextPin]()
					{
						return MutableThis->CanRemovePinBreakpoint(ContextPin);
					}),
					FGetActionCheckState(),
					FIsActionButtonVisible::CreateLambda([MutableThis, ContextPin]()
					{
						return MutableThis->CanRemovePinBreakpoint(ContextPin);
					})
				)
			);

			// Enable Breakpoint (pin)
			PinDebugSection.AddMenuEntry(
				"GF_EnableBreakpointOnPin",
				LOCTEXT("GF_EnableBreakpointOnPin_Label", "Enable Breakpoint"),
				LOCTEXT("GF_EnableBreakpointOnPin_Tooltip", "Enable breakpoint on this pin. "
												"Only enabled breakpoint can be triggered"),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateLambda([MutableThis, ContextPin]()
					{
						MutableThis->OnEnablePinBreakpointRequest(ContextPin);
					}),
					FCanExecuteAction::CreateLambda([MutableThis, ContextPin]()
					{
						return MutableThis->CanEnablePinBreakpoint(ContextPin);
					}),
					FGetActionCheckState(),
					FIsActionButtonVisible::CreateLambda([MutableThis, ContextPin]()
					{
						return MutableThis->CanEnablePinBreakpoint(ContextPin);
					})
				)
			);

			// Disable Breakpoint (pin)
			PinDebugSection.AddMenuEntry(
				"GF_DisableBreakpointOnPin",
				LOCTEXT("GF_DisableBreakpointOnPin_Label", "Disable Breakpoint"),
				LOCTEXT("GF_DisableBreakpointOnPin_Tooltip", "Disable breakpoint on this pin. "
												 "Disabled breakpoint cannot be triggered"),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateLambda([MutableThis, ContextPin]()
					{
						MutableThis->OnDisablePinBreakpointRequest(ContextPin);
					}),
					FCanExecuteAction::CreateLambda([MutableThis, ContextPin]()
					{
						return MutableThis->CanDisablePinBreakpoint(ContextPin);
					}),
					FGetActionCheckState(),
					FIsActionButtonVisible::CreateLambda([MutableThis, ContextPin]()
					{
						return MutableThis->CanDisablePinBreakpoint(ContextPin);
					})
				)
			);
		}
	}
	else if(Context->Node != nullptr)
	{
		// Node actions.
		{
			FToolMenuSection& GameFlowSection = Menu->AddSection("GameFlow", NSLOCTEXT("FGameFlowNode", "NodeContextAction", "Node actions"));
			GameFlowSection.AddMenuEntryWithCommandList(GraphNodeCommands.ValidateNode, ContextMenuCommands);
			GameFlowSection.AddMenuEntryWithCommandList(GraphNodeCommands.ReplaceNode, ContextMenuCommands);
			GameFlowSection.AddMenuEntryWithCommandList(GraphEditorCommands.ReconstructNodes, ContextMenuCommands);
			GameFlowSection.AddMenuEntryWithCommandList(GenericCommands.Delete, ContextMenuCommands);
			GameFlowSection.AddMenuEntryWithCommandList(GenericCommands.Copy, ContextMenuCommands);
			GameFlowSection.AddMenuEntryWithCommandList(GenericCommands.Paste, ContextMenuCommands);
		}

		// Debug actions.
		{
			FToolMenuSection& DebugSection = Menu->AddSection("DebugSection", NSLOCTEXT("FGameFlowNode", "NodeDebugContextAction", "Debug"));
			DebugSection.AddMenuEntryWithCommandList(GraphEditorCommands.AddBreakpoint, ContextMenuCommands);
			DebugSection.AddMenuEntryWithCommandList(GraphEditorCommands.RemoveBreakpoint, ContextMenuCommands);
			DebugSection.AddMenuEntryWithCommandList(GraphEditorCommands.EnableBreakpoint, ContextMenuCommands);
			DebugSection.AddMenuEntryWithCommandList(GraphEditorCommands.DisableBreakpoint, ContextMenuCommands);
		}
		
		// Utils.
		{
			FToolMenuSection& UtilsSection = Menu->AddSection("UtilsSection", NSLOCTEXT("FGameFlowNode", "NodeUtilsContextAction", "Utils"));

			/*
			// Node comment text box
			UtilsSection.AddEntry(FToolMenuEntry::InitWidget(
				"Comment Text",
				SNew(SEditableTextBox)
				.Text(FText::FromString(TEXT("")))
				.OnTextCommitted_Lambda(FOnTextCommitted::CreateUObject(this, &UGameFlowGraphNode::OnCommentTextCommitted)),
				FText::FromString(""),
				true
			));
			*/
		}
	} 
}

FEdGraphPinType UGameFlowGraphNode::GetGraphPinType() const
{
	FEdGraphPinType OutputPinInfo = {};
	OutputPinInfo.PinCategory = UEdGraphSchema_K2::PC_Exec;
	return OutputPinInfo;
}

void UGameFlowGraphNode::OnUpdateCommentText(const FString& NewComment)
{
	Super::OnUpdateCommentText(NewComment);
	NodeAsset->SavedNodeComment = NewComment;
}

void UGameFlowGraphNode::OnCommentBubbleToggled(bool bInCommentBubbleVisible)
{
	Super::OnCommentBubbleToggled(bInCommentBubbleVisible);
	NodeAsset->bIsCommentBubbleActive = bInCommentBubbleVisible;
}

bool UGameFlowGraphNode::IsCommentBubbleVisible() const
{
	return !NodeComment.IsEmpty() && NodeAsset->bIsCommentBubbleActive;
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
	// Find all input pins that have a connection.
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

bool UGameFlowGraphNode::CanEditNodeAsset() const
{
	return !(bIsReconstructing || bIsRebuilding || bPendingCompilation);
}

void UGameFlowGraphNode::Initialize()
{
	UGameFlowEditorSettings* Settings = UGameFlowEditorSettings::Get();
	// Get node asset info from config.
	Info = Settings->NodesTypes.FindChecked(NodeAsset->TypeName);
    
	// Listen to game flow asset events.
	NodeAsset->OnAssetRedirected.AddUObject(this, &UGameFlowGraphNode::ReconstructNode);
	NodeAsset->OnErrorEvent.AddUObject(this, &UGameFlowGraphNode::ReportError);
	NodeAsset->OnAssetExecuted.AddDynamic(this, &UGameFlowGraphNode::OnNodeAssetExecuted);

	TArray<UPinHandle*> InputPins = NodeAsset->GetPinsByDirection(EEdGraphPinDirection::EGPD_Input);
	// Listen for all the node input pins trigger event.
	for (UPinHandle* PinHandle : InputPins)
	{
		PinHandle->OnPinTriggered.AddDynamic(this, &UGameFlowGraphNode::TriggerBreakpoint);
	}

	TArray<UPinHandle*> OutputPins = NodeAsset->GetPinsByDirection(EEdGraphPinDirection::EGPD_Output);
	// Listen for all the node output pins trigger event.
	for (UPinHandle* PinHandle : OutputPins)
	{
		PinHandle->OnPinTriggered.AddDynamic(this, &UGameFlowGraphNode::TriggerBreakpoint);
	}
}

void UGameFlowGraphNode::ReconstructNode()
{
	bIsReconstructing = true;
	
	const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	
	const UGameFlowEditorSettings* GameFlowEditorSettings = UGameFlowEditorSettings::Get();
	Info = GameFlowEditorSettings->NodesTypes.FindRef(NodeAsset->TypeName);
	
	// Reconstruct pins outside copy-paste operations.
	if(!bIsBeingCopyPasted)
	{
		for (int i = Pins.Num() - 1; i >= 0; i--)
		{
			RemovePin(Pins[i]);
		}
		AllocateDefaultPins();

		const UGameFlowGraph& GameFlowGraph = *CastChecked<UGameFlowGraph>(GetGraph());
		// Recreate node connections for the reconstructed node.
		GraphSchema->RecreateNodeConnections(GameFlowGraph,
			this, TArray { EGPD_Input, EGPD_Output });
	}
	bIsReconstructing = false;
	// Notify node changed to redraw the node.
	GetGraph()->NotifyGraphChanged();
}

bool UGameFlowGraphNode::Modify(bool bAlwaysMarkDirty)
{
	if(NodeAsset != nullptr) NodeAsset->Modify();
	return Super::Modify(bAlwaysMarkDirty);
}

void UGameFlowGraphNode::OnNodeAssetExecuted(UInputPinHandle* InputPinHandle)
{
	bIsActive = true;
	TriggerBreakpoint();
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

UGameFlowNode* UGameFlowGraphNode::GetInspectedNodeInstance() const
{
	const UGameFlowAsset* DebuggedAssetInstance = CastChecked<UGameFlowGraph>(GetGraph())->DebuggedAssetInstance;
	if(DebuggedAssetInstance != nullptr)
	{
		return DebuggedAssetInstance->GetNodeByGUID(NodeGuid);
	}
	return nullptr;
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
	    GetGraph()->NotifyGraphChanged();	
	}
}

void UGameFlowGraphNode::OnRenameNode(const FString& NewName)
{
	Super::OnRenameNode(NewName);
	
	const FName NewNodeName (NewName);
	UGameFlowAsset* GameFlowAsset = NodeAsset->GetTypedOuter<UGameFlowAsset>();
	// Rename the node object only if the supplied name is unique.
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
		NodeAsset->Rename(*NewName, this, REN_DontCreateRedirectors);
	}
	else
	{
		// Notify the user there has been an error with node renaming.
		UE_LOG(LogGameFlow, Error, TEXT("Object name '%s' is already in use inside '%s' asset!"),
			*NewName, *GameFlowAsset->GetName())
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
					NodeAsset->AddPin(PinName, EGPD_Input);
					break;
				}
			
			// Add output pin to node asset.	
			case EGPD_Output:
				{
					TArray<FName> OutputPins = NodeAsset->GetOutputPinsNames();
					const FName PreviousName = OutputPins.Num() > 0 ? OutputPins.Last() : "None";
					PinName = CreateUniquePinName(PreviousName);
					NodeAsset->AddPin(PinName, EGPD_Output);
					break;
				}
		}
	}
	
	UEdGraphPin* Pin = nullptr;
	// Create the pin object only if name is valid.
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