// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	SLATE_END_ARGS()
    
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedPtr<GameFlowAssetToolkit> AssetEditor);

protected:
	virtual void OnGraphChanged(const FEdGraphEditAction& InAction) override;

private:
	/* Get the appearance of the Game Flow graph. */
	FGraphAppearanceInfo GetGraphAppearanceInfo();
};

