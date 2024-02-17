#pragma once

#include "GameFlowAsset.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Widget/SGameFlowGraph.h"

DECLARE_MULTICAST_DELEGATE(FOnAssetSaved)
DECLARE_MULTICAST_DELEGATE(FOnAssetCompile)

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
	virtual void CreateAssetMenu();
	virtual void CreateAssetToolbar();
    
	// --------------------- EDITOR ACTIONS ----------------------------------------------------------
	
	TSharedPtr<FUICommandList> CommandList;
	FOnAssetSaved OnAssetSavedCallback;
	FOnAssetCompile OnAssetCompileCallback;
	
	virtual void SaveAsset_Execute() override;
	virtual void TryCompiling();
	void CompileOnSaveToogle();
	void LiveCompileToogle();
	
	FORCEINLINE virtual bool CanCompile() { return true; }
	FORCEINLINE bool CanCompileOnSave() const { return Asset->bCompileOnSave; }
	FORCEINLINE bool CanLiveCompile() const { return Asset->bLiveCompile; }

public:
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	
	FORCEINLINE FOnAssetSaved& GetAssetSavedCallback() { return OnAssetSavedCallback; };
	FORCEINLINE FOnAssetCompile& GetAssetCompileCallback() { return OnAssetCompileCallback; }

private:
    /** Apply undo/redo registered actions to game flow editor. */
	void ExecuteUndoRedo();
	
	// ---------------------- EDITOR LAYOUT -------------------------------------------------------

protected:
	/* The layout which controls tabs and windows inside the editor. */
	TSharedPtr<FTabManager::FLayout> EditorLayout;
	
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual TSharedRef<FTabManager::FLayout> CreateEditorLayout();
	TSharedRef<IDetailsView> CreateAssetDetails();
    TSharedRef<IDetailsView> CreateAssetNodeDetails();
	
public:
	FORCEINLINE virtual FString GetWorldCentricTabPrefix() const override { return "Game Flow Asset"; }
	FORCEINLINE virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::Yellow; }

private:

	inline static const FName GraphTabName = "GraphTab";
	inline static const FName DetailsTabName = "DetailsTab";
	inline static const FName NodeDetailsTabName = "NodeDetailsTab";

	// -------------------------------------------------------------------------------------------------
};

