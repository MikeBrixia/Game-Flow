// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SGameFlowGraph.h"

#include "GameFlowEditor.h"
#include "GraphEditAction.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGameFlowGraph::Construct(const FArguments& InArgs, const TSharedPtr<GameFlowAssetToolkit> AssetEditor)
{
	// SGraphEditor init arguments.
	SGraphEditor::FArguments Arguments;
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

void SGameFlowGraph::OnGraphChanged(const FEdGraphEditAction& InAction)
{
	SGraphEditor::OnGraphChanged(InAction);
}

void SGameFlowGraph::OnSelectionChange(const TSet<UObject*>& Selection)
{
	UGameFlowGraph* Graph = CastChecked<UGameFlowGraph>(GetCurrentGraph());
	Graph->OnSelectionChanged(Selection);
}

FGraphAppearanceInfo SGameFlowGraph::GetGraphAppearanceInfo()
{
	FGraphAppearanceInfo GraphAppearanceInfo;
	GraphAppearanceInfo.CornerText = NSLOCTEXT("GameFlow","GameFlowGraph","Game Flow Editor");
	GraphAppearanceInfo.InstructionText = NSLOCTEXT("GameFlow", "GameFlowGraphInstruction", 
													"Create a Start node to use it as an entry point for your logic!");
	return GraphAppearanceInfo;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
