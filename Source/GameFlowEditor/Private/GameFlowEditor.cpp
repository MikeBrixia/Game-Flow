
#include "GameFlowEditor.h"
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
		
		// Register Game Flow assets.
		GameFlowAsset = MakeShared<FGameFlowAssetTypeAction>();
		AssetToolModule.Get().RegisterAssetTypeActions(GameFlowAsset.ToSharedRef());
		
		// Register Game Flow stylesheets.
		const FFlowNodeStyle& GameFlowNodeStyle = FFlowNodeStyle::GetDefault();
		FSlateStyleRegistry::RegisterSlateStyle(GameFlowNodeStyle.GetStyle());
        const FGameFlowEditorStyle& AssetEditorStyle = FGameFlowEditorStyle::GetDefault();
		FSlateStyleRegistry::RegisterSlateStyle(AssetEditorStyle.GetStyle());
		
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
}

void FGameFlowEditorModule::OnBlueprintCompiled()
{
	// Notify the graph editor about the compilation event.
	for (TObjectIterator<UGameFlowGraphNode> It; It; ++It)
	{
		UGameFlowGraphNode* Instance = *It;
		// Compile marked nodes
		if (Instance->bPendingCompilation)
		{
			Instance->OnAssetCompiled();
			// GF Compilation as finished.
			Instance->bPendingCompilation = false;
		}
	}
}

void FGameFlowEditorModule::OnBlueprintPreCompile(UBlueprint* Blueprint)
{
	// Fix up and reflect CDO changes to all node asset instances.
	for (TObjectIterator<UGameFlowNode> It; It; ++It)
	{
		// We're only interested in non-CDO objects.
		if (!It->HasAnyFlags(RF_ClassDefaultObject))
		{
			UGameFlowNode* Instance = *It;
			UGameFlowNode* Defaults = Cast<UGameFlowNode>(Instance->GetClass()->GetDefaultObject());

			// Propagate changes only to non-CDO objects.
			if (!Instance->IsTemplate())
			{
			    auto OldInputs = Instance->Inputs;
				Instance->Inputs = Defaults->Inputs;
				// Propagate input pins changes.
				for (auto& Pair : Instance->Inputs)
				{
					// CDO input pin name.
					FName PinName = Pair.Key;
					UInputPinHandle* PinHandle = OldInputs.FindRef(PinName);
					// Does the input handle associated with the CDO pin name already exist?
					if (PinHandle != nullptr)
					{
						// If true, simply migrate it to the instance input map.
						Pair.Value = PinHandle;
					}
					else
					{
						// If false, add a brand-new input pin with the CDO input pin name.
						Pair.Value = DuplicateObject(Pair.Value, Instance);
					}
				}
				
				auto OldOutputs = Instance->Outputs;
				Instance->Outputs = Defaults->Outputs;
				// Propagate output pin changes.
				for (auto& Pair : Instance->Outputs)
				{
					// CDO output pin name.
					FName PinName = Pair.Key;
					UOutPinHandle* PinHandle = OldOutputs.FindRef(PinName);
					// Does the pin handle associated with the CDO pin name already exists?
					if (PinHandle != nullptr)
					{
						// If true, simply migrate it to the instance input map.
						Pair.Value = PinHandle;
					}
					else
					{
						// If false, add a brand-new input pin with the CDO input pin name.
						Pair.Value = DuplicateObject(Pair.Value, Instance);
					}
				}
				Instance->Modify();
			}
		}
	}

	// Notify the graph editor about the compilation event.
	for (TObjectIterator<UGameFlowGraphNode> It; It; ++It)
	{
		UGameFlowGraphNode* Instance = *It;
		UGameFlowNode* ObservedNode = Instance->GetNodeAsset();
		// Is the observed node an instance of the compiled blueprint? If true, mark it for compilation.
		Instance->bPendingCompilation = ObservedNode != nullptr
		                                && ObservedNode->GetClass()->ClassGeneratedBy == Blueprint;
	}
}

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