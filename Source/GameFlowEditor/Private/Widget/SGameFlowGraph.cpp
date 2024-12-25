// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SGameFlowGraph.h"
#include "EdGraphUtilities.h"
#include "GameFlowEditor.h"
#include "GraphEditorActions.h"
#include "SGraphPanel.h"
#include "SlateOptMacros.h"
#include "Asset/Graph/Actions/FGameFlowNodeSchemaAction_PasteNode.h"
#include "Asset/Graph/Actions/GameFlowNodeSchemaAction_NewNode.h"
#include "Framework/Commands/GenericCommands.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGameFlowGraph::Construct(const FArguments& InArgs, const TSharedPtr<GameFlowAssetToolkit> AssetEditor)
{
	RegisterGraphCommands();
	
	// SGraphEditor init arguments.
	SGraphEditor::FArguments Arguments;
	Arguments._AdditionalCommands = CommandList;
    Arguments._GraphToEdit = InArgs._GraphToEdit;
	Arguments._Appearance = GetGraphAppearanceInfo();
	Arguments._GraphEvents = InArgs._GraphEvents;
	Arguments._AutoExpandActionMenu = true;
	Arguments._ShowGraphStateOverlay = true;
	
	// Initialize Graph editor callbacks.
	Arguments._GraphEvents.OnSelectionChanged = FOnSelectionChanged::CreateSP(this, &SGameFlowGraph::OnSelectionChange);
	
	// Create base SGraphEditor widget.
	SGraphEditor::Construct(Arguments);
}

UGameFlowGraph* SGameFlowGraph::GetGameFlowGraph() const
{
	return CastChecked<UGameFlowGraph>(GetCurrentGraph());
}

void SGameFlowGraph::RegisterGraphCommands()
{
	FGraphEditorCommands::Register();
	FGenericCommands::Register();
	
	const FGenericCommands& GenericCommands = FGenericCommands::Get();
	CommandList = MakeShareable(new FUICommandList);

	CommandList->MapAction(GenericCommands.Copy,
		                   FExecuteAction::CreateRaw(this, &SGameFlowGraph::OnCopyNode));
	CommandList->MapAction(GenericCommands.Paste,
						   FExecuteAction::CreateRaw(this, &SGameFlowGraph::OnPasteNode));
	CommandList->MapAction(GenericCommands.Delete,
						   FExecuteAction::CreateRaw(this, &SGameFlowGraph::OnDeleteNodes));
	
	// Generic Node commands
	CommandList->MapAction(GenericCommands.Undo,
	                       FExecuteAction::CreateSP(this, &SGameFlowGraph::UndoGraphAction));
	
	CommandList->MapAction(GenericCommands.Redo,
	                       FExecuteAction::CreateSP(this, &SGameFlowGraph::RedoGraphAction));
}

void SGameFlowGraph::OnSelectionChange(const TSet<UObject*>& Selection)
{
	const TSet<const UEdGraphNode*> SelectedNodes = reinterpret_cast<const TSet<const UEdGraphNode*>&>(Selection);
	UEdGraph* Graph = GetCurrentGraph();
	// Tell logic graph to select the already selected UI nodes.
	Graph->SelectNodeSet(SelectedNodes, true);
}


void SGameFlowGraph::OnCopyNode()
{
	const TSet<UGameFlowGraphNode*> SelectedNodes = reinterpret_cast<const TSet<UGameFlowGraphNode*>&>(GetSelectedNodes());
	// Prepare each node for copying operation.
	for(UGameFlowGraphNode* GraphNode : SelectedNodes)
	{
		GraphNode->PrepareForCopying();
	}

	const TSet<UObject*> Selection = reinterpret_cast<const TSet<UObject*>&>(GetSelectedNodes());
	FString ExportedText;
	
	// And then copy them to the clipboard.
	FEdGraphUtilities::ExportNodesToText(Selection, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

void SGameFlowGraph::OnPasteNode()
{
	FScopedTransaction Transaction(NSLOCTEXT("GameFlowEditor", "PasteNodes", "Paste nodes"));
	
	FString PastedData;
	// Read data from the clipboard.
	FPlatformApplicationMisc::ClipboardPaste(PastedData);

	UGameFlowGraph* Graph = CastChecked<UGameFlowGraph>(GetCurrentGraph());
	Graph->Modify();
	
	TSet<UEdGraphNode*> CopiedNodes;
	// Import nodes from clipboard text data and paste them.
	FEdGraphUtilities::ImportNodesFromText(Graph, PastedData, CopiedNodes);
	
	const TSet<UGameFlowGraphNode*> GraphNodes = reinterpret_cast<TSet<UGameFlowGraphNode*>&>(CopiedNodes);
	// Initialize pasted graph node.
	for(UGameFlowGraphNode* GraphNode : GraphNodes)
	{
		GraphNode->CreateNewGuid();
		
		Graph->GameFlowAsset->AddNode(GraphNode->GetNodeAsset());
		
		const FVector2D PastePosition = GetGraphPanel()->GetPastePosition();
		GraphNode->NodePosX = PastePosition.X;
		GraphNode->NodePosY = PastePosition.Y;
		GraphNode->PostPlacedNewNode();
	}
	
	NotifyGraphChanged();
}

void SGameFlowGraph::OnDeleteNodes()
{
	const TSet<UGameFlowGraphNode*> SelectedNodes = reinterpret_cast<const TSet<UGameFlowGraphNode*>&>(GetSelectedNodes());
	for(UGameFlowGraphNode* SelectedNode : SelectedNodes)
	{
		FGameFlowNodeSchemaAction_CreateOrDestroyNode DestroyNodeAction;
		DestroyNodeAction.PerformAction_DestroyNode(SelectedNode);
	}
}

void SGameFlowGraph::UndoGraphAction()
{
	GEditor->UndoTransaction();
}

void SGameFlowGraph::RedoGraphAction()
{
	GEditor->RedoTransaction();
}

FGraphAppearanceInfo SGameFlowGraph::GetGraphAppearanceInfo()
{
	FGraphAppearanceInfo GraphAppearanceInfo;
	GraphAppearanceInfo.CornerText = NSLOCTEXT("GameFlow", "GameFlowGraph", "Flow Graph");
	GraphAppearanceInfo.InstructionText = NSLOCTEXT("GameFlow", "GameFlowGraphInstruction", 
													"Create a Start node to use it as an entry point for your logic!");
	return GraphAppearanceInfo;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
