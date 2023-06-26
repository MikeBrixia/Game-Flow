#include "Asset/GameFlowAssetToolkit.h"

#include "GameFlowEditor.h"
#include "Asset/Graph/GameFlowGraph.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Kismet2/BlueprintEditorUtils.h"

GameFlowAssetToolkit::GameFlowAssetToolkit()
{
	
}

void GameFlowAssetToolkit::InitializeEditor(const TArray<UObject*>& InObjects)
{
	Asset = InObjects[0];
	
	// Create new layout.
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("Game Flow Layout");

	// Create asset editor primary area.
	const TSharedRef<FTabManager::FArea> PrimaryArea = FTabManager::NewPrimaryArea();

	// Create splitter and initialize it.
	const TSharedRef<FTabManager::FSplitter> Splitter = FTabManager::NewSplitter();
	Splitter->SetSizeCoefficient(0.6f);
	Splitter->SetOrientation(Orient_Horizontal);

	// Create UI Stack and initialize it.
	const TSharedRef<FTabManager::FStack> UIStack = FTabManager::NewStack();
	UIStack->SetSizeCoefficient(0.8f);
	UIStack->AddTab("GraphTab", ETabState::OpenedTab);

	// Create Details Stack and initialize it.
	const TSharedRef<FTabManager::FStack> DetailsStack = FTabManager::NewStack();
	DetailsStack->SetSizeCoefficient(0.2f);
	DetailsStack->AddTab("NodesTab", ETabState::OpenedTab);

	// Split tabs in the asset editor.
	Splitter->Split(UIStack);
	Splitter->Split(DetailsStack);

	// Split primary area from other splitted tabs.
	PrimaryArea->Split(Splitter);

	// Ad created area to the current layout
	Layout->AddArea(PrimaryArea);
	
	// Initialize Asset editor with the new layout.
	InitAssetEditor(EToolkitMode::Standalone, nullptr, "GameFlowAssetEditor",
				   Layout, true, true, InObjects);
}

void GameFlowAssetToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Initialize editor workspace.
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(INVTEXT("Game Flow Graph Editor"));
	
	// Create editor graph UI.
	CreateGraphTab(InTabManager);
}

void GameFlowAssetToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Unregisters all tabs
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner("GraphTab");
	InTabManager->UnregisterTabSpawner("NodesTab");
	
}

void GameFlowAssetToolkit::CreateGraphTab(const TSharedRef<FTabManager>& InTabManager)
{
	// Create the graph.
	const TObjectPtr<UEdGraph> Graph = FBlueprintEditorUtils::CreateNewGraph(Asset, "Flow Graph", UGameFlowGraph::StaticClass(),
							 										         UGameFlowGraphSchema::StaticClass());
	FGraphNodeCreator<UGameFlowGraphNode> Factory {*Graph};
	Factory.CreateNode(false);
	Factory.Finalize();
	
	// Create the graph tab.
	FTabSpawnerEntry& GraphTab = InTabManager->RegisterTabSpawner("GraphTab",
		FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		TSharedRef<SDockTab> Tab = SNew(SDockTab)
		[
			SNew(SGraphEditor)
			.GraphToEdit(Graph)
		];
		return Tab;
	}));
	GraphTab.SetDisplayName(INVTEXT("Flow Graph"));
	GraphTab.SetGroup(WorkspaceMenuCategory.ToSharedRef());
}



