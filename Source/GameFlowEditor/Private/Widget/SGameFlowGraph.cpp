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
    Arguments._GraphToEdit = AssetEditor->GetGraph();
	Arguments._Appearance = GetGraphAppearanceInfo();
	Arguments._GraphEvents = InArgs._GraphEvents;
	Arguments._AutoExpandActionMenu = true;
	Arguments._ShowGraphStateOverlay = true;
	// Create base SGraphEditor widget.
	SGraphEditor::Construct(Arguments);
}

void SGameFlowGraph::OnGraphChanged(const FEdGraphEditAction& InAction)
{
	SGraphEditor::OnGraphChanged(InAction);
	bool bDirty = InAction.Graph->MarkPackageDirty();
	if(bDirty)
	{
		UE_LOG(LogGameFlow, Display, TEXT("Graph package has been marked as dirty successfully"))
	}
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
