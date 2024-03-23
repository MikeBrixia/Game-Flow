#include "Asset/GameFlowAssetToolkit.h"
#include "GameFlowEditor.h"
#include "GraphEditorActions.h"
#include "Asset/GameFlowEditorCommands.h"
#include "Asset/Graph/GameFlowGraph.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Asset/Graph/Nodes/FGameFlowGraphNodeCommands.h"
#include "Kismet2/DebuggerCommands.h"
#include "Utils/GameFlowEditorSubsystem.h"
#include "Utils/GameFlowFactory.h"
#include "Widget/SGameFlowGraph.h"

GameFlowAssetToolkit::GameFlowAssetToolkit()
{
	this->CommandList = MakeShared<FUICommandList>();

	GEditor->RegisterForUndo(this);
}

void GameFlowAssetToolkit::InitEditor(const TArray<UObject*>& InObjects)
{
	// The asset being edited.
	Asset = CastChecked<UGameFlowAsset>(InObjects[0]);
	
	UGameFlowEditorSubsystem* EditorSubsystem = GEditor->GetEditorSubsystem<UGameFlowEditorSubsystem>();
	// Mark this editor as an active by registering it inside GameFlow editor subsystem.
	EditorSubsystem->RegisterActiveEditor(this);
	
	// Create the graph.
	UGameFlowGraph* Graph = UGameFlowFactory::CreateGraph<UGameFlowGraph, UGameFlowGraphSchema>(Asset);
	GraphWidget = SNew(SGameFlowGraph, SharedThis(this))
			       .GraphToEdit(Graph);
	
	// Initialize all different components of the Game Flow editor.
	EditorLayout = CreateEditorLayout();
	InitAssetEditor(EToolkitMode::Standalone, {}, "GameFlowAssetEditor",
				   EditorLayout->AsShared(), true, true, InObjects);
	ConfigureInputs();
	CreateAssetMenu();
	CreateAssetToolbar();
}

bool GameFlowAssetToolkit::OnRequestClose()
{
	UGameFlowEditorSubsystem* EditorSubsystem = GEditor->GetEditorSubsystem<UGameFlowEditorSubsystem>();
	// Mark this editor as an inactive by unregistering it from GameFlow editor subsystem.
	EditorSubsystem->UnregisterActiveEditor(this);
	
	UE_LOG(LogGameFlow, Display, TEXT("%s asset editor closed succesfully"), *Asset->GetName());
	return true;
}

TSharedRef<FTabManager::FLayout> GameFlowAssetToolkit::CreateEditorLayout()
{
	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("Game Flow Layout")
	->AddArea
	(
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Horizontal)
		->Split
		(
		FTabManager::NewStack()
			->SetSizeCoefficient(0.5)
			->AddTab(DetailsTabName, ETabState::OpenedTab)
		)
		->Split
		(
			FTabManager::NewStack()
			->SetSizeCoefficient(0.6)
			->AddTab(GraphTabName, ETabState::OpenedTab)
		)
		->Split
		(
		FTabManager::NewStack()
			  ->SetSizeCoefficient(0.5)
			  ->AddTab(NodeDetailsTabName, ETabState::OpenedTab)
		)
	);
	
	return Layout;
}

TSharedRef<IDetailsView> GameFlowAssetToolkit::CreateAssetDetails()
{
	// Create and initialize details tab.
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	FDetailsViewArgs DetailsViewArgs;
	// Initialize details view settings.
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	const TSharedRef<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	// Set the object to be inspected by the details tab.
	DetailsView->SetObjects(TArray<UObject*>{ Asset });

	return DetailsView;
}

TSharedRef<IDetailsView> GameFlowAssetToolkit::CreateAssetNodeDetails()
{
	// Create and initialize details tab.
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	// Initialize details view settings.
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
	const TSharedRef<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	
	TArray<UObject*> SelectedNodes = GraphWidget->GetSelectedNodes().Array();
	if(SelectedNodes.Num() == 1)
	{
		// Set the object to be inspected by the details tab.
		DetailsView->SetObjects(TArray<UObject*>{ SelectedNodes[0] });
	}
	
	return DetailsView;
}

void GameFlowAssetToolkit::ConfigureInputs()
{
	// Get all Game Flow editor commands.
	const FGameFlowEditorCommands& GameFlowCommands = FGameFlowEditorCommands::Get();
	
	// Engine's Play commands.
	ToolkitCommands->Append(FPlayWorldCommands::GlobalPlayWorldActions.ToSharedRef());
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
		Section.AddMenuEntryWithCommandList(GameFlowCommands.CompileAsset, ToolkitCommands);
		Section.AddMenuEntryWithCommandList(GameFlowCommands.CompileOnSave, ToolkitCommands);
		Section.AddMenuEntryWithCommandList(GameFlowCommands.LiveCompile, ToolkitCommands);
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

void GameFlowAssetToolkit::SaveAsset_Execute()
{
	// If there's at least one listener, broadcast save asset event.
	if(OnAssetSavedCallback.IsBound())
	{
		OnAssetSavedCallback.Broadcast();
	}

	FAssetEditorToolkit::SaveAsset_Execute();
}

void GameFlowAssetToolkit::PostUndo(bool bSuccess)
{
	FEditorUndoClient::PostUndo(bSuccess);
	ExecuteUndoRedo();
}

void GameFlowAssetToolkit::PostRedo(bool bSuccess)
{
	FEditorUndoClient::PostRedo(bSuccess);
	ExecuteUndoRedo();
}

void GameFlowAssetToolkit::ExecuteUndoRedo()
{
	GraphWidget->ClearSelectionSet();
	GraphWidget->NotifyGraphChanged();
	FSlateApplication::Get().DismissAllMenus();
}

void GameFlowAssetToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Initialize editor workspace.
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(INVTEXT("Game Flow Graph Editor"));
	
	NodesDetailsView = CreateAssetNodeDetails();
	// Create palette tab, responsible for displaying all available GameFlow nodes.
	FTabSpawnerEntry& NodeDetailsTab = InTabManager->RegisterTabSpawner(NodeDetailsTabName,
		FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		TSharedRef<SDockTab> Tab = SNew(SDockTab)
		[
			NodesDetailsView->AsShared()
		];
		return Tab;
	}));
	NodeDetailsTab.SetDisplayName(INVTEXT("Node Details"));
	NodeDetailsTab.SetGroup(WorkspaceMenuCategory.ToSharedRef());
	
	// Register asset graph tab. This area will be used by developers to create GameFlow logic.
	FTabSpawnerEntry& GraphTab = InTabManager->RegisterTabSpawner(GraphTabName,
		FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		TSharedRef<SDockTab> Tab = SNew(SDockTab)
		[
			GraphWidget->AsShared()
		];
		return Tab;
	}));
	GraphTab.SetDisplayName(INVTEXT("Flow Graph"));
	GraphTab.SetGroup(WorkspaceMenuCategory.ToSharedRef());

	// Register details tab responsible for letting developers edit asset exposed properties.
	FTabSpawnerEntry& DetailsTab = InTabManager->RegisterTabSpawner(DetailsTabName,
		FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
		{
			TSharedRef<SDockTab> Tab = SNew(SDockTab)
			[
				CreateAssetDetails()
		    ];    
			return Tab;
		}));	
	DetailsTab.SetDisplayName(INVTEXT("Details"));
	DetailsTab.SetGroup(WorkspaceMenuCategory.ToSharedRef());
	
}

void GameFlowAssetToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Unregisters all tabs
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner(GraphTabName);
	InTabManager->UnregisterTabSpawner(DetailsTabName);
    InTabManager->UnregisterTabSpawner(NodeDetailsTabName);
	
	// Unregister editor menu and toolbar.
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	FGameFlowEditorCommands::Unregister();
}





