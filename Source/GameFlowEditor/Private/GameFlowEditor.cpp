
#include "GameFlowEditor.h"
#include "EdGraphUtilities.h"
#include "GraphEditorActions.h"
#include "ISettingsModule.h"
#include "Asset/GameFlowEditorCommands.h"
#include "Asset/GameFlowEditorStyleWidgetStyle.h"
#include "Asset/Graph/Nodes/FGameFlowGraphNodeCommands.h"
#include "Config/GameFlowEditorSettings.h"
#include "Config/GameFlowSettings.h"
#include "HAL/PlatformFileManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widget/Nodes/FlowNodeStyle.h"

#define LOCTEXT_NAMESPACE "FGameFlowEditorModule"

DEFINE_LOG_CATEGORY(LogGameFlow)

EAssetTypeCategories::Type FGameFlowEditorModule::GameFlowCategory = EAssetTypeCategories::None;

// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
void FGameFlowEditorModule::StartupModule()
{
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

	// On startup we need to forward some data to the runtime settings,
	// given the plugin setup this is the best way to share it.
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

void FGameFlowEditorModule::InitializeCppScriptTemplates()
{
	const FString ScriptTemplatesPath = GetScriptTemplatesPath();
	const FString EngineEditorScriptTemplatesPath = GetEngineScriptTemplatesPath();
	
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	const bool bCopyResult = FileManager.CopyDirectoryTree(*EngineEditorScriptTemplatesPath, *ScriptTemplatesPath, false);
	
	const TCHAR* CopyMsg = bCopyResult? TEXT("Script templates copied succesfully to engine template folder!") :
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

	UE_LOG(LogGameFlow, Display, TEXT("At init time: %f"), RuntimeSettings->WireHighlightDuration)
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