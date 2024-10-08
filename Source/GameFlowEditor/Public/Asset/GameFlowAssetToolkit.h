﻿#pragma once

#include "GameFlowAsset.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Widget/SGameFlowGraph.h"

DECLARE_MULTICAST_DELEGATE(FOnAssetSaved)

/** The editor of a Game Flow asset. */
class GameFlowAssetToolkit : public FAssetEditorToolkit, FEditorUndoClient
{
public:
	GameFlowAssetToolkit();

	// -------------------- GAME FLOW EDITOR METADATA ----------------------------------------------
private:

	/* Asset inspected by this editor. */
	TObjectPtr<UGameFlowAsset> Asset;

public:
	TSharedPtr<IDetailsView> NodesDetailsView;
	TSharedPtr<SGameFlowGraph> GraphWidget;
	
	FORCEINLINE UObject* GetAsset() const { return Asset; }
	FORCEINLINE virtual FText GetBaseToolkitName() const override { return INVTEXT("GameFlowToolkit"); }
	FORCEINLINE virtual FName GetToolkitFName() const override { return "Game Flow Toolkit"; }
	
	// -------------------- GAME FLOW EDITOR CORE -------------------------------------------------------
	
	void InitEditor(const TArray<UObject*>& InObjects);

protected:
	
	virtual bool OnRequestClose() override;
	virtual void ConfigureInputs();
	virtual void CreateGraph();
	virtual void CreateAssetMenu();
	virtual void CreateAssetToolbar();
    
	// --------------------- EDITOR ACTIONS ----------------------------------------------------------
	
	TSharedPtr<FUICommandList> CommandList;
	FOnAssetSaved OnAssetSavedCallback;
	
	virtual void SaveAsset_Execute() override;
    void OnValidateRequest();
	void OnDebugRequest();
	void OnPostPIEStarted(bool bStarted);
	void OnPIEFinish(bool bFinished);
	void OnPIEDebuggedInstanceInvalidated(UGameFlowAsset* DebuggedInstance);
    void SelectPIEAssetInstance(UGameFlowAsset* AssetInstance);
	void UnselectPIEWorld();
	void UnselectPIEAssetInstance();
   
public:
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	
	FORCEINLINE FOnAssetSaved& GetAssetSavedCallback() { return OnAssetSavedCallback; };
    FORCEINLINE UToolMenu* GetToolbar() const;
	
private:
	/** The selected world inside PIE menu. */
	UWorld* PIE_SelectedWorld;
	/** The selected asset instance inside PIE menu. */
	UGameFlowAsset* PIE_SelectedAssetInstance;
	
    /** Apply undo/redo registered actions to game flow editor. */
	void ExecuteUndoRedo();
	/** Display selected nodes inside node details panel. */
	void DisplaySelectedNodes(TSet<UGameFlowGraphNode*> Nodes);
	
	// ---------------------- EDITOR LAYOUT -------------------------------------------------------

protected:
	/* The layout which controls tabs and windows inside the editor. */
	TSharedPtr<FTabManager::FLayout> EditorLayout;
	
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual TSharedRef<FTabManager::FLayout> CreateEditorLayout();
	TSharedRef<IDetailsView> CreateAssetDetails();
    TSharedRef<IDetailsView> CreateAssetNodeDetails();
	
	TSharedRef<SWidget> CreatePIEDebugToolbarSection();
	TSharedRef<SWidget> BuildSelectPIEWorldMenu();
	TSharedRef<SWidget> BuildSelectAssetInstanceMenu();
	
public:
	FORCEINLINE virtual FString GetWorldCentricTabPrefix() const override { return "Game Flow Asset"; }
	FORCEINLINE virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::Yellow; }

private:

	inline static const FName GraphTabName = "GraphTab";
	inline static const FName DetailsTabName = "DetailsTab";
	inline static const FName NodeDetailsTabName = "NodeDetailsTab";

	// -------------------------------------------------------------------------------------------------
};


