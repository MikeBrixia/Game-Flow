// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GraphEditor.h"
#include "Asset/Graph/GameFlowGraph.h"
#include "Asset/GameFlowAssetToolkit.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Widget class for GameFlow graph editor.
 */
class GAMEFLOWEDITOR_API SGameFlowGraph : public SGraphEditor
{
public:
	
	SLATE_BEGIN_ARGS(SGameFlowGraph)
		{
		   
		}
	   SLATE_ARGUMENT(FGraphEditorEvents, GraphEvents)
	   SLATE_ARGUMENT(UGameFlowGraph*, GraphToEdit)
	SLATE_END_ARGS()
    
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedPtr<GameFlowAssetToolkit> AssetEditor);

	UGameFlowGraph* GetGameFlowGraph() const;
	
protected:

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	virtual void RegisterGraphCommands();
    virtual void OnSelectionChange(const TSet<UObject*>& Selection);
	
	virtual void OnCopyNodes();
	virtual void OnPasteNodes();
    virtual void OnDeleteNodes();
	
	void UndoGraphAction();
	void RedoGraphAction();
	
	/* Get the appearance of the Game Flow graph. */
	virtual FGraphAppearanceInfo GetGraphAppearanceInfo();

private:
	TSharedPtr<FUICommandList> CommandList;
	FVector2D LastClickPosition;
};


