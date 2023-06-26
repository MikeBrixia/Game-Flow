
#include "GameFlowEditor.h"

#include "EdGraphUtilities.h"
#include "Asset/Graph/GameFlowConnectionDrawingPolicy.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widget/Nodes/FlowNodeStyle.h"

#define LOCTEXT_NAMESPACE "FGameFlowEditorModule"
DEFINE_LOG_CATEGORY(LogGameFlow)

EAssetTypeCategories::Type FGameFlowEditorModule::GameFlowCategory = EAssetTypeCategories::None;

void FGameFlowEditorModule::StartupModule()
{
	// Initialize asset tools modules
	const FAssetToolsModule& AssetToolModule = FAssetToolsModule::GetModule();
	
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		// Register game flow as a standalone category inside the asset panel.
		GameFlowCategory = AssetToolModule.Get().RegisterAdvancedAssetCategory("Game_Flow", FText::FromString("Game Flow"));
		
		// Register Game Flow assets.
		GameFlowAsset = MakeShared<FGameFlowAssetTypeAction>();
		AssetToolModule.Get().RegisterAssetTypeActions(GameFlowAsset.ToSharedRef());

		// Register game flow node stylesheet.
		const FFlowNodeStyle& GameFlowNodeStyle = FFlowNodeStyle::GetDefault();
		FSlateStyleRegistry::RegisterSlateStyle(GameFlowNodeStyle.GetStyle());

		// Register Game Flow graph visual factories.
		FEdGraphUtilities::RegisterVisualPinConnectionFactory(MakeShareable(new FGraphPanelPinConnectionFactory));
	}
}

void FGameFlowEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(GameFlowAsset.ToSharedRef());

		// Unregister game flow node stylesheet.
		const FFlowNodeStyle& GameFlowNodeStyle = FFlowNodeStyle::GetDefault();
		FSlateStyleRegistry::UnRegisterSlateStyle(GameFlowNodeStyle.GetStyle());
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGameFlowEditorModule, GameFlowEditor)