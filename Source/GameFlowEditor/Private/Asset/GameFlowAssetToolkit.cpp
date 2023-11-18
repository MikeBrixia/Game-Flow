#include "Asset/GameFlowAssetToolkit.h"

#include "GameFlowEditor.h"
#include "GraphEditorActions.h"
#include "Asset/GameFlowEditorCommands.h"
#include "Asset/Graph/GameFlowGraph.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Kismet2/DebuggerCommands.h"
#include "Utils/GameFlowEditorUtils.h"
#include "Utils/GameFlowFactory.h"

GameFlowAssetToolkit::GameFlowAssetToolkit()
{
	this->CommandList = MakeShared<FUICommandList>();

	// Register editor commands.
	FGraphEditorCommands::Register();
	FGameFlowEditorCommands::Register();
}

void GameFlowAssetToolkit::InitEditor(const TArray<UObject*>& InObjects)
{
	// The asset being edited.
	Asset = InObjects[0];
	
	// Create the graph.
	Graph = UGameFlowFactory::CreateGraph<UGameFlowGraph, UGameFlowGraphSchema>(Asset);
	
	// Create editor tabs.
	const TSharedRef<FTabManager::FLayout> Layout = GameFlowEditorUtils::CreateEditorTabs();
	
	// Initialize Asset editor with the new layout.
	InitAssetEditor(EToolkitMode::Standalone, {}, "GameFlowAssetEditor",
				   Layout, true, true, InObjects);
	
	// Configure all the editor inputs.
	ConfigureInputs();

	// Create asset menu for this editor.
	CreateAssetMenu();

	// Create asset toolbar for this editor.
	CreateAssetToolbar();
	
}

void GameFlowAssetToolkit::ConfigureInputs()
{
	// Get all Game Flow editor commands.
	const FGameFlowEditorCommands& GameFlowCommands = FGameFlowEditorCommands::Get();
	
	// Engine's Play commands.
	ToolkitCommands->Append(FPlayWorldCommands::GlobalPlayWorldActions.ToSharedRef());

	// Compiling input action.
	ToolkitCommands->MapAction(GameFlowCommands.CompileAsset,
		                       FExecuteAction::CreateRaw(this, &GameFlowAssetToolkit::OnCompile),
		                       FCanExecuteAction::CreateRaw(this, &GameFlowAssetToolkit::CanCompile));
}

void GameFlowAssetToolkit::CreateAssetMenu()
{
	// Get all Game Flow editor commands.
	const FGameFlowEditorCommands GameFlowCommands = FGameFlowEditorCommands::Get();
	
	// Get 'Asset' tool menu of this editor.
	FName MenuName = FName(GetToolMenuName().ToString() + ".Asset");
	UToolMenu* AssetMenu = UToolMenus::Get()->ExtendMenu(MenuName);
	// Have we found the tool menu?
	if(AssetMenu != nullptr)
	{
		// If true, Create a new Game Flow section inside the Asset tool menu.
		FToolMenuSection& Section = AssetMenu->FindOrAddSection("AssetFlow");
		Section.Label = INVTEXT("Game Flow");
		// Add compile command to Asset tool menu, inside 'Game Flow' section.
		Section.AddMenuEntryWithCommandList(GameFlowCommands.CompileAsset, CommandList);
	}
}

void GameFlowAssetToolkit::CreateAssetToolbar()
{
	// Get all Game Flow editor commands.
	const FGameFlowEditorCommands GameFlowCommands = FGameFlowEditorCommands::Get();
	
	// Try finding Game Flow asset toolbar.
	FName MenuName = FName(GetToolMenuToolbarName());
	UToolMenu* AssetToolbar = UToolMenus::Get()->ExtendMenu(MenuName);
	
	// Have we found the asset toolbar?
	if(AssetToolbar != nullptr)
	{
		// If true, Create a new Game Flow section inside the Asset tool menu.
		FToolMenuSection& GameFlowSection = AssetToolbar->FindOrAddSection("Game_flow");
		GameFlowSection.AddEntry(FToolMenuEntry::InitToolBarButton(GameFlowCommands.CompileAsset));

		// Create toolbar play section(Play section is the part of the toolbar of Unreal which
		// contains actions to start the editor game application).
		FToolMenuSection& PlaySection = AssetToolbar->FindOrAddSection("Play");
		FPlayWorldCommands::BuildToolbar(PlaySection);
	}
}

void GameFlowAssetToolkit::OnCompile()
{
	UGameFlowGraph* GameFlowGraph = Cast<UGameFlowGraph>(Graph);
	checkf(GameFlowGraph, TEXT("Asset could not be compiled! Graph is nullptr"));
	// Begins asset compilation.
	GameFlowGraph->CompileGraph(Asset);
}

FGraphAppearanceInfo GameFlowAssetToolkit::GetGraphAppearance()
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
	FTabSpawnerEntry& PaletteTab = InTabManager->RegisterTabSpawner("NodesTab",
		FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		TSharedRef<SDockTab> Tab = SNew(SDockTab)
		[
			SNew(STextBlock)
			.Text(INVTEXT("Graph nodes list..."))
		];
		return Tab;
	}));
	PaletteTab.SetDisplayName(INVTEXT("Palette"));
	PaletteTab.SetGroup(WorkspaceMenuCategory.ToSharedRef());
	
	// Create the graph tab.
	FTabSpawnerEntry& GraphTab = InTabManager->RegisterTabSpawner("GraphTab",
		FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		TSharedRef<SDockTab> Tab = SNew(SDockTab)
		[
			SNew(SGraphEditor)
			.GraphToEdit(Graph)
			.Appearance(GetGraphAppearance())
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

	// Unregister editor menu and toolbar.
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	FGameFlowEditorCommands::Unregister();
}




