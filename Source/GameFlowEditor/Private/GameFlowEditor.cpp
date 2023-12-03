
#include "GameFlowEditor.h"
#include "EdGraphUtilities.h"
#include "ISettingsModule.h"
#include "Asset/GameFlowEditorStyleWidgetStyle.h"
#include "Config/GameFlowEditorSettings.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widget/Nodes/FlowNodeStyle.h"

#define LOCTEXT_NAMESPACE "FGameFlowEditorModule"
DEFINE_LOG_CATEGORY(LogGameFlow)

EAssetTypeCategories::Type FGameFlowEditorModule::GameFlowCategory = EAssetTypeCategories::None;

// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
void FGameFlowEditorModule::StartupModule()
{
	// Initialize asset tools modules
	const FAssetToolsModule& AssetToolModule = FAssetToolsModule::GetModule();
	
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		// Register game flow as a standalone category inside the asset panel.
		GameFlowCategory = AssetToolModule.Get().RegisterAdvancedAssetCategory("Game_Flow", FText::FromString("Game Flow"));
		
		// Register Game Flow assets.
		GameFlowAsset = MakeShared<FGameFlowAssetTypeAction>();
		AssetToolModule.Get().RegisterAssetTypeActions(GameFlowAsset.ToSharedRef());

		// Register game flow stylesheets.
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
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGameFlowEditorModule, GameFlowEditor)