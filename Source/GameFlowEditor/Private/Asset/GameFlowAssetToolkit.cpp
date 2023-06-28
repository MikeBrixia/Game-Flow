#include "Asset/GameFlowAssetToolkit.h"
#include "GraphEditorActions.h"
#include "Asset/Graph/GameFlowGraph.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Utils/GameFlowFactory.h"

GameFlowAssetToolkit::GameFlowAssetToolkit()
{
	this->CommandList = MakeShared<FUICommandList>();
}

void GameFlowAssetToolkit::InitEditor(const TArray<UObject*>& InObjects)
{
	// The asset being edited.
	Asset = InObjects[0];
	
	// Create the graph.
	Graph = UGameFlowFactory::CreateGraph<UGameFlowGraph, UGameFlowGraphSchema>(Asset);

	// Create editor tabs.
	const TSharedRef<FTabManager::FLayout> Layout = CreateEditorTabs();

	// Configure all the editor inputs.
	ConfigureInputs();
	
	// Initialize Asset editor with the new layout.
	InitAssetEditor(EToolkitMode::Standalone, nullptr, "GameFlowAssetEditor",
				   Layout, true, true, InObjects);
}

void GameFlowAssetToolkit::ConfigureInputs()
{
	FGraphEditorCommands::Register();
	const FGraphEditorCommandsImpl& GraphEditorCommands = FGraphEditorCommands::Get();
	
	//CommandList->MapAction(GraphEditorCommands.SummonCreateNodeMenu,
	//	                   FExecuteAction::CreateSP(this, &)
}

TSharedRef<FTabManager::FLayout> GameFlowAssetToolkit::CreateEditorTabs()
{
	// Create new layout.
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("Game Flow Layout");

	// Create asset editor primary area.
	TSharedRef<FTabManager::FArea> PrimaryArea = FTabManager::NewPrimaryArea();

	// Create splitter and initialize it.
	TSharedRef<FTabManager::FSplitter> Splitter = FTabManager::NewSplitter();
	Splitter->SetSizeCoefficient(0.8f);
	Splitter->SetOrientation(Orient_Horizontal);

	// Create UI Stack and initialize it.
	TSharedRef<FTabManager::FStack> GraphTab = FTabManager::NewStack();
	GraphTab ->SetSizeCoefficient(0.8f);
	GraphTab ->AddTab("GraphTab", ETabState::OpenedTab);

	// Create Details Stack and initialize it.
	TSharedRef<FTabManager::FStack> NodesTab = FTabManager::NewStack();
	NodesTab->SetSizeCoefficient(0.2f);
	NodesTab->AddTab("NodesTab", ETabState::OpenedTab);

	// Split Graph and details tab.
	Splitter->Split(GraphTab);
	Splitter->Split(NodesTab);
	PrimaryArea->Split(Splitter);

	// Add the layout to the asset editor.
	Layout->AddArea(PrimaryArea);

	return Layout;
}

FGraphAppearanceInfo GameFlowAssetToolkit::GetGraphApperance()
{
	FGraphAppearanceInfo GraphAppearanceInfo;
	GraphAppearanceInfo.CornerText = NSLOCTEXT("GameFlow","GameFlowGraph","Game Flow Editor");
	GraphAppearanceInfo.InstructionText = NSLOCTEXT("GameFlow", "GameFlowGraphInstruction", 
                                                    "Create a Start node to use it as an entry point for your logic!");
	return GraphAppearanceInfo;
}

void GameFlowAssetToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Initialize editor workspace.
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(INVTEXT("Game Flow Graph Editor"));
	
	// Create the details tab.
	FTabSpawnerEntry& DetailsTab = InTabManager->RegisterTabSpawner("NodesTab",
		FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		TSharedRef<SDockTab> Tab = SNew(SDockTab)
		[
			SNew(STextBlock)
			.Text(INVTEXT("Graph nodes list..."))
		];
		return Tab;
	}));
	DetailsTab.SetDisplayName(INVTEXT("Nodes"));
	DetailsTab.SetGroup(WorkspaceMenuCategory.ToSharedRef());
	
	// Create the graph tab.
	FTabSpawnerEntry& GraphTab = InTabManager->RegisterTabSpawner("GraphTab",
		FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		TSharedRef<SDockTab> Tab = SNew(SDockTab)
		[
			SNew(SGraphEditor)
			.GraphToEdit(Graph)
			.Appearance(GetGraphApperance())
			.ShowGraphStateOverlay(true)
		];
		return Tab;
	}));
	GraphTab.SetDisplayName(INVTEXT("Flow Graph"));
	GraphTab.SetGroup(WorkspaceMenuCategory.ToSharedRef());
}

void GameFlowAssetToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Unregisters all tabs
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner("GraphTab");
	InTabManager->UnregisterTabSpawner("NodesTab");
}




