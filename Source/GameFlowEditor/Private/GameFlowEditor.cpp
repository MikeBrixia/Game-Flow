
#include "GameFlowEditor.h"

#include "DiffResults.h"
#include "EdGraphUtilities.h"
#include "GraphEditorActions.h"
#include "ISettingsModule.h"
#include "Asset/GameFlowEditorCommands.h"
#include "Asset/GameFlowEditorStyleWidgetStyle.h"
#include "Asset/Graph/Nodes/FGameFlowGraphNodeCommands.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Config/GameFlowEditorSettings.h"
#include "Config/GameFlowSettings.h"
#include "HAL/PlatformFileManager.h"
#include "Nodes/GameFlowNode.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widget/Nodes/FlowNodeStyle.h"

#define LOCTEXT_NAMESPACE "FGameFlowEditorModule"

DEFINE_LOG_CATEGORY(LogGameFlow)

EAssetTypeCategories::Type FGameFlowEditorModule::GameFlowCategory = EAssetTypeCategories::None;

// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
void FGameFlowEditorModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FGameFlowEditorModule::OnPostEngineInit);
	
	// Register game flow editor commands.
	FGraphEditorCommands::Register();
	FGameFlowEditorCommands::Register();
	FGameFlowGraphNodeCommands::Register();
	
	// Initialize asset tools modules
	const FAssetToolsModule& AssetToolModule = FAssetToolsModule::GetModule();
	
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		// Register game flow as a standalone category inside the asset panel.
		GameFlowCategory = AssetToolModule.Get().RegisterAdvancedAssetCategory("Game_Flow", FText::FromString("Game Flow"));
		
		// Register Game Flow stylesheets.
		const FFlowNodeStyle& GameFlowNodeStyle = FFlowNodeStyle::GetDefault();
		FSlateStyleRegistry::RegisterSlateStyle(GameFlowNodeStyle.GetStyle());
		const FGameFlowEditorStyle& AssetEditorStyle = FGameFlowEditorStyle::GetDefault();
		FSlateStyleRegistry::RegisterSlateStyle(AssetEditorStyle.GetStyle());
		
		// Register Game Flow assets.
		GameFlowAsset = MakeShared<FGameFlowAssetTypeAction>();
		AssetToolModule.Get().RegisterAssetTypeActions(GameFlowAsset.ToSharedRef());
		
		// Register Game Flow graph visual factories. these factories will be used as the default
		// strategy to instantiate all the involved items.
		FEdGraphUtilities::RegisterVisualPinConnectionFactory(MakeShareable(new FGraphPanelPinConnectionFactory));
	}
	
	// Add GameFlow to project settings.
	if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Game Flow",
			LOCTEXT("RuntimeSettingsName", "Game Flow"), LOCTEXT("RuntimeSettingsDescription", "Configure Game Flow editor properties"),
			 GetMutableDefault<UGameFlowEditorSettings>());
	}
	
	// Add Game Flow script templates to the engine.
	InitializeCppScriptTemplates();

	// On startup, we need to forward some data to the runtime settings;
	// given the plugin setup, this is the best way to share it.
	ForwardEditorSettingsToRuntimeSettings();
}

// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
// we call this function before unloading the module.
void FGameFlowEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(GameFlowAsset.ToSharedRef());

		// Unregister game flow node stylesheet.
		const FFlowNodeStyle& GameFlowNodeStyle = FFlowNodeStyle::GetDefault();
		FSlateStyleRegistry::UnRegisterSlateStyle(GameFlowNodeStyle.GetStyle());
	}
	
	// Remove GameFlow from project settings.
	if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Game Flow");
	}

	// Remove all game flow cpp script templates from the engine.
	RemoveCppScriptTemplates();
}

void FGameFlowEditorModule::OnPostEngineInit()
{
	// Hookup to editor blueprint compilation events.
	GEditor->OnBlueprintCompiled().AddRaw(this, &FGameFlowEditorModule::OnBlueprintCompiled);
	GEditor->OnBlueprintPreCompile().AddRaw(this, &FGameFlowEditorModule::OnBlueprintPreCompile);

#if WITH_HOT_RELOAD || WITH_LIVE_CODING
	FCoreUObjectDelegates::ReloadCompleteDelegate.AddRaw(this, &FGameFlowEditorModule::OnHotReload);
#endif
}

void FGameFlowEditorModule::OnBlueprintCompiled()
{
	// Notify the graph editor about the compilation event.
	for (TObjectIterator<UGameFlowGraphNode> It; It; ++It)
	{
		UGameFlowGraphNode* Instance = *It;

		UGameFlowNode* ObservedNode = Instance->GetNodeAsset();

		if (ObservedNode == nullptr) continue;
		
		UGameFlowNode* Default = ObservedNode->GetClass()->GetDefaultObject<UGameFlowNode>();
            
		TArray<FDiffSingleResult> InputPinsDiff;
		FDiffResults DiffResults = ObservedNode->PinsDiff(Default,InputPinsDiff, EGPD_Input);
		// If input pins have been changed inside the CDO, update the observed node using the REINST obj.
		if (DiffResults.HasFoundDiffs())
		{
			PostCompilePinsFixup(InputPinsDiff, ObservedNode, EGPD_Input);
		}

		TArray<FDiffSingleResult> OutputPinsDiff;
		DiffResults = ObservedNode->PinsDiff(Default,OutputPinsDiff, EGPD_Output);
		// If output pins have been changed inside the CDO, update the observed node.
		if (DiffResults.HasFoundDiffs())
		{
			PostCompilePinsFixup(OutputPinsDiff, ObservedNode, EGPD_Output);
		}
		
		ObservedNode->TypeName = Default->TypeName;
		// Compile marked nodes
		Instance->OnAssetCompiled();
	}
}

void FGameFlowEditorModule::OnBlueprintPreCompile(UBlueprint* Blueprint)
{
	// Fix up all game flow nodes before blueprint compilation.
	for (TObjectIterator<UGameFlowGraphNode> It; It; ++It)
	{
		UGameFlowGraphNode* Instance = *It;
		UGameFlowNode* ObservedNode = Instance->GetNodeAsset();
		// Is the observed node an instance of the compiled blueprint? If true, mark it for compilation.
		if (ObservedNode != nullptr
			&& ObservedNode->GetClass()->ClassGeneratedBy == Blueprint)
		{
			Instance->MarkNodeAsPendingCompilation();
		}
	}
}

#if WITH_HOT_RELOAD || WITH_LIVE_CODING

void FGameFlowEditorModule::OnHotReload(EReloadCompleteReason ReloadCompleteReason)
{
	TArray<UClass*> ClassesToRebuild;
	
	for (TObjectIterator<UGameFlowNode> It; It; ++It)
	{
		UGameFlowNode* ObservedNode = *It;

		// Re-instance of the observed node class with the post-compilation CDO.
		UGameFlowNode* REINST_Instance = NewObject<UGameFlowNode>(GetTransientPackage(), ObservedNode->GetClass(), FName("GF_REINST_" + ObservedNode->GetName()), RF_Transient);
		
		TArray<FDiffSingleResult> InputPinsDiff;
		FDiffResults DiffResults = ObservedNode->PinsDiff(REINST_Instance,InputPinsDiff, EGPD_Input);
		// If input pins have been changed inside the CDO, update the observed node using the REINST obj.
		if (DiffResults.HasFoundDiffs())
		{
			ClassesToRebuild.Add(ObservedNode->GetClass());
			PostCompilePinsFixup(InputPinsDiff, ObservedNode, EGPD_Input);
		}

		TArray<FDiffSingleResult> OutputPinsDiff;
		DiffResults = ObservedNode->PinsDiff(REINST_Instance,OutputPinsDiff, EGPD_Output);
		// If output pins have been changed inside the CDO, update the observed node using the REINST obj.
		if (DiffResults.HasFoundDiffs())
		{
			ClassesToRebuild.Add(ObservedNode->GetClass());
			PostCompilePinsFixup(OutputPinsDiff, ObservedNode, EGPD_Output);
		}
		ObservedNode->TypeName = REINST_Instance->TypeName;
	}

	// After logical node fixups, update the graph nodes associated with objs derived from the modified UClasses.
	for (TObjectIterator<UGameFlowGraphNode> It; It; ++It)
	{
		UGameFlowGraphNode* Instance = *It;
		// Update only graph nodes observing updated UClasses nodes.
		if (Instance->GetNodeAsset() != nullptr &&
			ClassesToRebuild.Contains(Instance->GetNodeAsset()->GetClass()))
		{
			Instance->MarkNodeAsPendingCompilation();
			Instance->OnAssetCompiled(); 
		}
	}
}

void FGameFlowEditorModule::PostCompilePinsFixup(TArray<FDiffSingleResult> Diff, UGameFlowNode* Node, EEdGraphPinDirection PinDirection)
{
	// Fixup all detected differences between the nodes pins.
	for (const FDiffSingleResult& SingleResult : Diff)
	{
		if (SingleResult.Diff == EDiffType::OBJECT_REQUEST_DIFF)
		{
			// If we have a pin that the reinstance does not have, remove it from the observed node.
			if (SingleResult.Category == EDiffType::ADDITION)
			{
				Node->RemovePin(FName(SingleResult.DisplayString.ToString()), PinDirection);
			}
			// If we don't have a pin that the reinstance have, add it to the observed node.
			else if (SingleResult.Category == EDiffType::SUBTRACTION)
			{
				Node->AddPin(FName(SingleResult.DisplayString.ToString()), PinDirection);
			}
		}
	}
}

#endif

void FGameFlowEditorModule::InitializeCppScriptTemplates()
{
	const FString ScriptTemplatesPath = GetScriptTemplatesPath();
	const FString EngineEditorScriptTemplatesPath = GetEngineScriptTemplatesPath();
	
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	const bool bCopyResult = FileManager.CopyDirectoryTree(*EngineEditorScriptTemplatesPath, *ScriptTemplatesPath, false);
	
	const TCHAR* CopyMsg = bCopyResult? TEXT("Script templates copied successfully to engine template folder!") :
										TEXT("Script templates could not be copied to engine template folder");
	UE_LOG(LogGameFlow, Display, TEXT("%s"), CopyMsg);
}

void FGameFlowEditorModule::ForwardEditorSettingsToRuntimeSettings()
{
	UGameFlowSettings* RuntimeSettings = UGameFlowSettings::Get();
	UGameFlowEditorSettings* EditorSettings = UGameFlowEditorSettings::Get();
	
	TArray<FName> Options;
	EditorSettings->NodesTypes.GenerateKeyArray(Options);
	RuntimeSettings->Options = Options;
	RuntimeSettings->WireHighlightDuration = EditorSettings->WireHighlightDuration;
}

void FGameFlowEditorModule::RemoveCppScriptTemplates()
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	
	const FString EngineScriptTemplatesPath = GetEngineScriptTemplatesPath();
	TArray<FString> TemplateFilesToRemove = {EngineScriptTemplatesPath / "UGameFlowNodeClass.h.template",
	                                         EngineScriptTemplatesPath / "UGameFlowNodeClass.cpp.template"};
	for(const FString& TemplateFile : TemplateFilesToRemove)
	{
		FileManager.DeleteFile(*TemplateFile);
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGameFlowEditorModule, GameFlowEditor)