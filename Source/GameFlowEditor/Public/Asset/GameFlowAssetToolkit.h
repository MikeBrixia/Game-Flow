#pragma once

#include "Toolkits/AssetEditorToolkit.h"

/** Class responsible for creating and initializing the
 *  Game Flow Asset editor.
 */
class GameFlowAssetToolkit : public FAssetEditorToolkit
{
public:

	GameFlowAssetToolkit();

	virtual void InitializeEditor(const TArray<UObject*>& InObjects);
	
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	
	FORCEINLINE virtual FText GetBaseToolkitName() const override { return FText::FromString("GameFlowToolkit"); }
	FORCEINLINE virtual FName GetToolkitFName() const override { return "Game Flow Toolkit"; }
	FORCEINLINE virtual FString GetWorldCentricTabPrefix() const override { return "Game Flow Asset"; }
	FORCEINLINE virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::Yellow; }

private:

	/* The actual asset from which we're going to create the editor. */
	TObjectPtr<UObject> Asset;
	
	/* Create and initialize the editor graph tab and UI. */
	virtual void CreateGraphTab(const TSharedRef<FTabManager>& InTabManager);
};
