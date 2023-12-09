#pragma once

#include "GameFlowAsset.h"
#include "Graph/GameFlowGraph.h"
#include "Toolkits/AssetEditorToolkit.h"

/** The editor of a Game Flow asset. */
class GameFlowAssetToolkit : public FAssetEditorToolkit
{
	
public:

	GameFlowAssetToolkit();
	
	void InitEditor(const TArray<UObject*>& InObjects);
	
	// ---------------------- EDITOR LAYOUT -------------------------------------------------------

private:

	/* The layout which controls tabs and windows inside the editor. */
	TSharedPtr<FTabManager::FLayout> EditorLayout;
	
	/* Register editor tab widget and create the contained widgets. */
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

	/* Unregister editor tab widget and all the contained UI elements. */
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	
	TSharedRef<FTabManager::FLayout> CreateEditorLayout();
	TSharedRef<IDetailsView> CreateAssetDetailsTab();
	FORCEINLINE virtual FString GetWorldCentricTabPrefix() const override { return "Game Flow Asset"; }
	FORCEINLINE virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::Yellow; }
	
	// -------------------- EDITOR METADATA ----------------------------------------------
public:
	
	FORCEINLINE UObject* GetAsset() const { return Asset; }
	FORCEINLINE virtual FText GetBaseToolkitName() const override { return INVTEXT("GameFlowToolkit"); }
	FORCEINLINE virtual FName GetToolkitFName() const override { return "Game Flow Toolkit"; }
    FORCEINLINE UGameFlowGraph* GetGraph() const { return Graph; }

	// ----------------- EDITOR COMMANDS ---------------------------------------------------
	virtual void SaveAsset_Execute() override;

	// ---------------------------------------------------------------------
	
protected:
	
	/** Configure asset editor inputs. All menu and toolbar
	 * actions will be registered inside this method. */
	virtual void ConfigureInputs();

	/** Create Game Flow asset editor menu. */
	virtual void CreateAssetMenu();

	/** Create Game Flow asset editor toolbar. */
	virtual void CreateAssetToolbar();

	/** Called when the user wants to compile the editor asset.*/
	virtual void OnCompile();
	
	/** Is the editor asset currently able to be compiled? */
	FORCEINLINE virtual bool CanCompile()
	{
		return true;
	}

	/* The actual asset from which we're going to create the editor. */
	TObjectPtr<UGameFlowAsset> Asset;

	/** The graph being created. */
	TObjectPtr<UGameFlowGraph> Graph;

	/** The list of command inputs the user can execute. */
	TSharedPtr<FUICommandList> CommandList;

private:

	inline static const FName GraphTabName = "GraphTab";
	inline static const FName DetailsTabName = "DetailsTab";
	inline static const FName NodeDetailsTabName = "NodeDetailsTab";
	inline static const FName PaletteTabName = "PaletteTab";
};

