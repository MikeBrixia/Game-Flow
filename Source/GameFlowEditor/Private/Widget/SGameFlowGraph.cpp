// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SGameFlowGraph.h"
#include "EdGraphUtilities.h"
#include "GameFlowEditor.h"
#include "GraphEditorActions.h"
#include "ScopedTransaction.h"
#include "SGraphPanel.h"
#include "SlateOptMacros.h"
#include "Asset/Graph/Actions/GameFlowNodeSchemaAction_NewNode.h"
#include "Framework/Commands/GenericCommands.h"
#include "HAL/PlatformApplicationMisc.h"

FVector2D SGameFlowGraph::SelectionRectCenter = FVector2D::ZeroVector;

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
	Arguments._GraphEvents.OnSelectionChanged.BindSP(this, &SGameFlowGraph::OnSelectionChange);
	
	// Create base SGraphEditor widget.
	SGraphEditor::Construct(Arguments);
}

UGameFlowGraph* SGameFlowGraph::GetGameFlowGraph() const
{
	return CastChecked<UGameFlowGraph>(GetCurrentGraph());
}

FReply SGameFlowGraph::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	LastClickPosition = MouseEvent.GetScreenSpacePosition();
	return SGraphEditor::OnMouseButtonDown(MyGeometry, MouseEvent);
}

void SGameFlowGraph::RegisterGraphCommands()
{
	FGraphEditorCommands::Register();
	FGenericCommands::Register();
	
	const FGenericCommands& GenericCommands = FGenericCommands::Get();
	CommandList = MakeShareable(new FUICommandList);

	CommandList->MapAction(GenericCommands.Copy,
		                   FExecuteAction::CreateSP(this, &SGameFlowGraph::OnCopyNodes));
	CommandList->MapAction(GenericCommands.Paste,
						   FExecuteAction::CreateSP(this, &SGameFlowGraph::OnPasteNodes));
	CommandList->MapAction(GenericCommands.Delete,
						   FExecuteAction::CreateSP(this, &SGameFlowGraph::OnDeleteNodes));
	
	// Generic Node commands
	CommandList->MapAction(GenericCommands.Undo,
	                       FExecuteAction::CreateSP(this, &SGameFlowGraph::UndoGraphAction));
	
	CommandList->MapAction(GenericCommands.Redo,
	                       FExecuteAction::CreateSP(this, &SGameFlowGraph::RedoGraphAction));
}

void SGameFlowGraph::OnSelectionChange(const TSet<UObject*>& Selection)
{
	TSet<const UEdGraphNode*> SelectedNodes;
	for (UObject* Obj : Selection)
	{
		if (const UEdGraphNode* Node = Cast<UEdGraphNode>(Obj))
		{
			SelectedNodes.Add(Node);
		}
	}

	if (UEdGraph* Graph = GetCurrentGraph())
	{
		Graph->SelectNodeSet(SelectedNodes);
	}
}

void SGameFlowGraph::OnGraphChanged(const struct FEdGraphEditAction& InAction)
{
	SGraphEditor::OnGraphChanged(InAction);
}

void SGameFlowGraph::OnCopyNodes()
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

	FSlateRect SelectionRect;
	GetBoundsForSelectedNodes(SelectionRect, 0);
	SelectionRectCenter = SelectionRect.GetCenter();
}

void SGameFlowGraph::OnPasteNodes()
{
	FScopedTransaction Transaction(NSLOCTEXT("GameFlowEditor", "PasteNodes", "Paste nodes"));
	FString PastedData;
	// Read data from the clipboard.
	FPlatformApplicationMisc::ClipboardPaste(PastedData);
	
	UGameFlowGraph* Graph = CastChecked<UGameFlowGraph>(GetCurrentGraph());
	Graph->Modify();
	Graph->GameFlowAsset->Modify();
	
	TSet<UEdGraphNode*> CopiedNodes;
	// Import nodes from clipboard text data and paste them.
	FEdGraphUtilities::ImportNodesFromText(Graph, PastedData, CopiedNodes);
	
	const TArray<UGameFlowGraphNode*> GraphNodes = reinterpret_cast<TSet<UGameFlowGraphNode*>&>(CopiedNodes).Array();
	SGraphPanel* GraphPanel = GetGraphPanel();
	
	// Initialize pasted graph node.
	for(int i = 0; i < GraphNodes.Num(); ++i)
	{
		UGameFlowGraphNode* GraphNode = GraphNodes[i];
		
		const FVector2D NodeOldPosition = FVector2D(GraphNode->NodePosX, GraphNode->NodePosY);
		FVector2D PastePosition = GraphPanel->GetPastePosition();
		FVector2D DeltaPosition = NodeOldPosition - SelectionRectCenter;
		
		FVector2D FinalPosition = PastePosition + DeltaPosition;
		GraphNode->NodePosX = FinalPosition.X;
		GraphNode->NodePosY = FinalPosition.Y;
		
		UGameFlowNode* NodeAsset = GraphNode->GetNodeAsset();
		GraphNode->CreateNewGuid();
		// Override with graph node GUID.
		NodeAsset->GUID = GraphNode->NodeGuid;
		Graph->GameFlowAsset->AddNode(GraphNode->GetNodeAsset());
		
		// Copy paste operation has ended.
		GraphNode->bIsBeingCopyPasted = false;
	}
	
	Graph->GameFlowAsset->MarkPackageDirty();
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
