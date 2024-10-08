﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SGameFlowGraph.h"
#include "GraphEditorActions.h"
#include "SGraphPanel.h"
#include "SlateOptMacros.h"
#include "Asset/Graph/Actions/GameFlowNodeSchemaAction_NewNode.h"
#include "Framework/Commands/GenericCommands.h"

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
	GraphAppearanceInfo.CornerText = NSLOCTEXT("GameFlow", "GameFlowGraph", "Game Flow Editor");
	GraphAppearanceInfo.InstructionText = NSLOCTEXT("GameFlow", "GameFlowGraphInstruction", 
													"Create a Start node to use it as an entry point for your logic!");
	return GraphAppearanceInfo;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
