#include "Asset/GameFlowAssetToolkit.h"

#include "EditorUndoClient.h"
#include "GameFlowEditor.h"
#include "GameFlowSubsystem.h"
#include "PropertyEditorModule.h"
#include "ToolMenus.h"
#include "Asset/GameFlowEditorCommands.h"
#include "Asset/Graph/GameFlowGraph.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Config/GameFlowEditorSettings.h"
#include "Framework/Application/SlateApplication.h"
#include "Kismet2/DebuggerCommands.h"
#include "Utils/GameFlowFactory.h"
#include "Widget/SGameFlowGraph.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SComboButton.h"

GameFlowAssetToolkit::GameFlowAssetToolkit()
{
	this->CommandList = MakeShared<FUICommandList>();
	this->PIE_SelectedWorld = nullptr;
	this->PIE_SelectedAssetInstance = nullptr;
}

void GameFlowAssetToolkit::InitEditor(const TArray<UObject*>& InObjects)
{
	// The asset being edited.
	Asset = CastChecked<UGameFlowAsset>(InObjects[0]);
	
	// Create the logical and graphical game flow graph.
	CreateGraph();
	
	// Initialize all different components of the Game Flow editor.
	EditorLayout = CreateEditorLayout();
	InitAssetEditor(EToolkitMode::Standalone, {}, "GameFlowAssetEditor",
				   EditorLayout->AsShared(), true, true, InObjects);

	// Create editor interaction components.
	ConfigureInputs();
	CreateAssetMenu();
	CreateAssetToolbar();
    
	GEditor->RegisterForUndo(this);
	FEditorDelegates::PostPIEStarted.AddSP(this, &GameFlowAssetToolkit::OnPostPIEStarted);
	FEditorDelegates::EndPIE.AddSP(this, &GameFlowAssetToolkit::OnPIEFinish);
}

#if ENGINE_MINOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 6

bool GameFlowAssetToolkit::OnRequestClose(EAssetEditorCloseReason InCloseReason)
{
	UE_LOG(LogGameFlow, Display, TEXT("%s asset editor closed succesfully"), *Asset->GetName());
	return FAssetEditorToolkit::OnRequestClose(InCloseReason);
}

#elif ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION < 6 

bool GameFlowAssetToolkit::OnRequestClose()
{
	UE_LOG(LogGameFlow, Display, TEXT("%s asset editor closed succesfully"), *Asset->GetName());
	return true;
}

#endif

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

TSharedRef<SWidget> GameFlowAssetToolkit::CreatePIEDebugToolbarSection()
{
		return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString("World: "))
			.Margin(FMargin(0, 8))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SComboButton)
			.OnGetMenuContent(this, &GameFlowAssetToolkit::BuildSelectPIEWorldMenu)
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text_Lambda([this]
				{
					return PIE_SelectedWorld != nullptr? FText::FromString(PIE_SelectedWorld->GetMapName())
												   : FText::FromString("None");
				})
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Instance: "))
			.Margin(FMargin(6, 8))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SComboButton)
			.OnGetMenuContent(this, &GameFlowAssetToolkit::BuildSelectAssetInstanceMenu)
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text_Lambda([this]
				{
					return PIE_SelectedAssetInstance != nullptr? FText::FromString(PIE_SelectedAssetInstance->GetName())
												   : FText::FromString("None");
				})
			]
		];
}

void GameFlowAssetToolkit::ConfigureInputs()
{
	const FGameFlowEditorCommands& GameFlowCommands = FGameFlowEditorCommands::Get();
	
	// Game Flow commands.
	ToolkitCommands->MapAction(GameFlowCommands.ValidateAsset,
		FExecuteAction::CreateRaw(this, &GameFlowAssetToolkit::OnValidateRequest));
	ToolkitCommands->MapAction(GameFlowCommands.DebugAsset,
		FExecuteAction::CreateRaw(this, &GameFlowAssetToolkit::OnDebugRequest),
		FCanExecuteAction::CreateRaw(this, &GameFlowAssetToolkit::CanEnableDebug),
		FIsActionChecked::CreateRaw(this, &GameFlowAssetToolkit::IsDebugEnabled));
	
	// Engine's Play commands.
	ToolkitCommands->Append(FPlayWorldCommands::GlobalPlayWorldActions.ToSharedRef());
}

void GameFlowAssetToolkit::CreateGraph()
{
	// Create the graph.
	UGameFlowGraph* Graph = UGameFlowFactory::CreateGraph<UGameFlowGraph, UGameFlowGraphSchema>(Asset);
	
	// Listen for game flow graph events.
	Graph->OnGraphNodesSelected.BindRaw(this, &GameFlowAssetToolkit::DisplaySelectedNodes);
	Graph->OnBreakpointHitRequest.BindRaw(this, &GameFlowAssetToolkit::OnBreakpointHit);
    Graph->EditorToolkit = this;
	
	// Create UI widget from logical graph.
	GraphWidget = SNew(SGameFlowGraph, SharedThis(this))
				   .GraphToEdit(Graph);
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

		Section.AddMenuEntryWithCommandList(GameFlowCommands.ValidateAsset, ToolkitCommands);
	}
}

void GameFlowAssetToolkit::CreateAssetToolbar()
{
	// Get all Game Flow editor commands.
	const FGameFlowEditorCommands GameFlowCommands = FGameFlowEditorCommands::Get();
	
	// Try finding Game Flow asset toolbar.
	UToolMenu* AssetToolbar = GetToolbar();
	
	// Have we found the asset toolbar?
	if(AssetToolbar != nullptr)
	{
		// If true, Create a new Game Flow section inside the Asset tool menu.
		FToolMenuSection& GameFlowSection = AssetToolbar->FindOrAddSection("Game_flow");
		GameFlowSection.AddEntry(FToolMenuEntry::InitToolBarButton(GameFlowCommands.ValidateAsset));
		GameFlowSection.AddEntry(FToolMenuEntry::InitToolBarButton(GameFlowCommands.DebugAsset));
		
		// Create toolbar play section(Play section is the part of the toolbar of Unreal which
		// contains actions to start the editor game application).
		FToolMenuSection& PlaySection = AssetToolbar->FindOrAddSection("Play");
		FPlayWorldCommands::BuildToolbar(PlaySection);
	}
}

void GameFlowAssetToolkit::OnBreakpointHit(UGameFlowGraphNode* GraphNode, UEdGraphPin* GraphPin)
{
	const TSharedPtr<SDockTab> FlowGraphTab = TabManager->FindExistingLiveTab(GraphTabName);
	if(FlowGraphTab.IsValid())
	{
		// First focus on the flow graph editor tab.
		FlowGraphTab->DrawAttention();
		// Then if pin was the source of the breakpoint hit, jump to it
		if(GraphPin != nullptr)
		{
			GraphWidget->JumpToPin(GraphPin);
		}
		// Otherwise jump to the node
		else
		{
			GraphWidget->JumpToNode(GraphNode);
		}
	}
}

void GameFlowAssetToolkit::SaveAsset_Execute()
{
	FAssetEditorToolkit::SaveAsset_Execute();
	
	// If there's at least one listener, broadcast save asset event.
	UGameFlowGraph* Graph = CastChecked<UGameFlowGraph>(GraphWidget->GetCurrentGraph());
    Graph->OnSaveGraph();
    
	UE_LOG(LogGameFlow, Display, TEXT("%s asset saved successfully!"), *Graph->GameFlowAsset->GetName())
}

void GameFlowAssetToolkit::OnValidateRequest()
{
	// If there's at least one listener, broadcast save asset event.
	UGameFlowGraph* Graph = CastChecked<UGameFlowGraph>(GraphWidget->GetCurrentGraph());
    Graph->OnValidateGraph();

	UE_LOG(LogGameFlow, Display, TEXT("%s asset validated successfully!"), *Asset->GetName())
}

void GameFlowAssetToolkit::OnDebugRequest()
{
	bDebugEnabled = !bDebugEnabled;
	UGameFlowGraph* Graph = CastChecked<UGameFlowGraph>(GraphWidget->GetCurrentGraph());
	Graph->OnDebugModeUpdated(bDebugEnabled);
}

bool GameFlowAssetToolkit::IsDebugEnabled() const
{
	return bDebugEnabled;
}

bool GameFlowAssetToolkit::CanEnableDebug() const
{
	return true;
}

void GameFlowAssetToolkit::OnPostPIEStarted(bool bStarted)
{
	// Try finding Game Flow asset toolbar.
	UToolMenu* EditorToolbar = GetToolbar();
	if(EditorToolbar != nullptr)
	{
		// By default select the first PIE world in the hierarchy.
		const FWorldContext* WorldContext = GEditor->GetWorldContextFromPIEInstance(0);
		PIE_SelectedWorld = WorldContext->World();
		
		const UGameFlowSubsystem* Subsystem = PIE_SelectedWorld->GetGameInstance()->GetSubsystem<UGameFlowSubsystem>();
		const TArray<UGameFlowAsset*> InstancedAssets = Subsystem->GetRunningFlows();
		// If there is at least one instance of the inspected game flow asset, select the first you can find by default.
		if(InstancedAssets.Num() > 0)
		{
			UGameFlowAsset* InstancedAsset = Subsystem->GetRunningFlowByArchetype(Asset->GetArchetype());
			SelectPIEAssetInstance(InstancedAsset);
		}
		
		const TSharedRef<SWidget> PIE_DebugSection = CreatePIEDebugToolbarSection();
		// Get all Game Flow editor commands.
		const FGameFlowEditorCommands GameFlowCommands = FGameFlowEditorCommands::Get();
		FToolMenuSection& PlaySection = EditorToolbar->FindOrAddSection("PIE Debug");
		
		const FToolMenuEntry& SelectPIEWorldInstanceEntry = FToolMenuEntry::InitWidget("Select PIE World",
			PIE_DebugSection, FText::GetEmpty());

		PlaySection.AddEntry(SelectPIEWorldInstanceEntry);
	}
}

void GameFlowAssetToolkit::OnPIEFinish(bool bFinished)
{
	UToolMenu* EditorToolbar = GetToolbar();
	if(EditorToolbar != nullptr)
	{
		EditorToolbar->RemoveSection("PIE Debug");
		UnselectPIEWorld();
	}
}

void GameFlowAssetToolkit::OnPIEDebuggedInstanceInvalidated(UGameFlowAsset* DebuggedInstance)
{
	if(PIE_SelectedWorld != nullptr)
	{
		// Timer necessary when terminating a game flow asset execution. This wil add
		// a small delay to allow graph drawing policy to highlight executed connections
		// before invalidating the debug asset instance following asset execution end.
		FTimerHandle TimerHandle;
		PIE_SelectedWorld->GetTimerManager().SetTimer(TimerHandle, [this]
		{
			PIE_SelectedAssetInstance->OnFinish.RemoveAll(this);
	        this->PIE_SelectedAssetInstance = nullptr;
	        // Invalidate graph debugged instance.
	        GraphWidget->GetGameFlowGraph()->SetDebuggedInstance(nullptr);
		}
		, UGameFlowEditorSettings::Get()->WireHighlightDuration, false);
	}
}

void GameFlowAssetToolkit::SelectPIEAssetInstance(UGameFlowAsset* AssetInstance)
{
	if(PIE_SelectedAssetInstance != nullptr)
	{
		PIE_SelectedAssetInstance->OnFinish.RemoveAll(this);
	}
	AssetInstance->OnFinish.AddRaw(this, &GameFlowAssetToolkit::OnPIEDebuggedInstanceInvalidated);

	UGameFlowGraph* GraphObj = GraphWidget->GetGameFlowGraph();
	// Override PIE selected asset instance with new one.
	GraphObj->SetDebuggedInstance(AssetInstance);

	PIE_SelectedAssetInstance = AssetInstance;
}

void GameFlowAssetToolkit::UnselectPIEWorld()
{
	if(PIE_SelectedWorld != nullptr && PIE_SelectedAssetInstance != nullptr)
	{
		UnselectPIEAssetInstance();
		// Unselect PIE World instance.
		PIE_SelectedWorld = nullptr;
	}
}

void GameFlowAssetToolkit::UnselectPIEAssetInstance()
{
	if(PIE_SelectedAssetInstance != nullptr)
	{
		PIE_SelectedAssetInstance->OnFinish.RemoveAll(this);
		
		UGameFlowGraph* GraphObj = GraphWidget->GetGameFlowGraph();
		GraphObj->SetDebuggedInstance(nullptr);
		
		PIE_SelectedAssetInstance = nullptr;
	}
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

UToolMenu* GameFlowAssetToolkit::GetToolbar() const
{
	// Try finding Game Flow asset toolbar.
	const FName MenuName = FName(GetToolMenuToolbarName());
	return UToolMenus::Get()->ExtendMenu(MenuName);
}

TSharedRef<SWidget> GameFlowAssetToolkit::BuildSelectPIEWorldMenu()
{
	FMenuBuilder OptionsMenuBuilder(true, nullptr);
	
	const int32 PIESessionsCount = GEditor->GetPlayInEditorSessionInfo()->PIEInstanceCount;
	// Add all PIE instances worlds as options inside the dropdown menu.
	for(int i = 0; i < PIESessionsCount; i++)
	{
		const FWorldContext* WorldContext = GEditor->GetWorldContextFromPIEInstance(i);
		UWorld* PIE_PlayWorld = WorldContext->World();
		OptionsMenuBuilder.AddMenuEntry(FText::FromString(PIE_PlayWorld->GetMapName()), FText::GetEmpty(),
			FSlateIcon(), FExecuteAction::CreateLambda([this, PIE_PlayWorld]
			{
				// Make sure to unselect all PIE world and asset at once.
				if(PIE_PlayWorld == nullptr)
				{
					UnselectPIEWorld();
				}
				this->PIE_SelectedWorld = PIE_PlayWorld;
			}));
	}
	return OptionsMenuBuilder.MakeWidget();
}

TSharedRef<SWidget> GameFlowAssetToolkit::BuildSelectAssetInstanceMenu()
{
	FMenuBuilder OptionsMenuBuilder(true, nullptr);

	if(PIE_SelectedWorld != nullptr)
	{
		const UGameFlowSubsystem* Subsystem = PIE_SelectedWorld->GetGameInstance()->GetSubsystem<UGameFlowSubsystem>();
		for(UGameFlowAsset* Instance : Subsystem->GetRunningFlows())
		{
			OptionsMenuBuilder.AddMenuEntry(FText::FromString(Instance->GetName()), FText::GetEmpty(),
			FSlateIcon(), FExecuteAction::CreateLambda([this, Instance]
			{
				SelectPIEAssetInstance(Instance);
			}));
		}
	}

	return OptionsMenuBuilder.MakeWidget();
}

void GameFlowAssetToolkit::ExecuteUndoRedo()
{
	GraphWidget->ClearSelectionSet();
	GraphWidget->NotifyGraphChanged();
	FSlateApplication::Get().DismissAllMenus();
}

void GameFlowAssetToolkit::DisplaySelectedNodes(TSet<const UGameFlowGraphNode*> Nodes)
{
	// Array of selected nodes assets.
	TArray<UObject*> SelectedAssets;
	
	// Build selected nodes assets array.
	for(const UGameFlowGraphNode* SelectedNode : Nodes)
	{
		UGameFlowNode* NodeAsset = SelectedNode->GetNodeAsset();
		SelectedAssets.Add(NodeAsset);
	}
    
	// Inspect selected nodes inside editor nodes details view.
	NodesDetailsView->SetObjects(SelectedAssets);
}

void GameFlowAssetToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// Initialize editor workspace.
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(INVTEXT("Game Flow Graph Editor"));
	
	NodesDetailsView = CreateAssetNodeDetails();
	// Create palette tab, responsible for displaying all available GameFlow nodes.
	FTabSpawnerEntry& NodeDetailsTab = InTabManager->RegisterTabSpawner(NodeDetailsTabName,
		FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs&)
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
		FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs&)
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
		FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs&)
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
}





