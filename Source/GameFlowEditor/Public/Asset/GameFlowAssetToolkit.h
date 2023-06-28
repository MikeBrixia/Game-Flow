#pragma once

#include "Toolkits/AssetEditorToolkit.h"

/** The editor of a Game Flow asset. */
class GameFlowAssetToolkit : public FAssetEditorToolkit
{
public:

	GameFlowAssetToolkit();

	/** Initialize game flow asset editor. */
	void InitEditor(const TArray<UObject*>& InObjects);
	/** Register editor the tab widget and create the contained widgets. */
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	/** Unregister editor the tab widget and all the contained UI elements. */
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
    
	/** Get the asset inspected by this editor. */
	FORCEINLINE UObject* GetAsset() const{ return Asset; }
	FORCEINLINE virtual FText GetBaseToolkitName() const override { return INVTEXT("GameFlowToolkit"); }
	FORCEINLINE virtual FName GetToolkitFName() const override { return "Game Flow Toolkit"; }
	FORCEINLINE virtual FString GetWorldCentricTabPrefix() const override { return "Game Flow Asset"; }
	FORCEINLINE virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::Yellow; }

private:
	
	/* The actual asset from which we're going to create the editor. */
	TObjectPtr<UObject> Asset;

	/** The graph being created. */
	TObjectPtr<UEdGraph> Graph;

	/** The list of command inputs the user can execute. */
	TSharedPtr<FUICommandList> CommandList;
	
	/** Configure asset editor inputs. */
	void ConfigureInputs();
	/** Create all the Game Flow editor tabs. */
	TSharedRef<FTabManager::FLayout> CreateEditorTabs();
	/** Get the appearance of the Game Flow graph. */
	FGraphAppearanceInfo GetGraphApperance();
};
