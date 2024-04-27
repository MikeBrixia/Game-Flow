#include "Asset/GameFlowAssetTypeAction.h"

#include "GameFlowAsset.h"
#include "GameFlowEditor.h"
#include "Asset/GameFlowAssetToolkit.h"
#include "Asset/GameFlowEditorStyleWidgetStyle.h"
#include "Styling/SlateStyleRegistry.h"

FGameFlowAssetTypeAction::FGameFlowAssetTypeAction()
{
}

uint32 FGameFlowAssetTypeAction::GetCategories()
{
	return FGameFlowEditorModule::GameFlowCategory;
}

FText FGameFlowAssetTypeAction::GetName() const
{
	return FText::FromString("Game Flow Asset");
}

UClass* FGameFlowAssetTypeAction::GetSupportedClass() const
{
	return UGameFlowAsset::StaticClass();
}

FColor FGameFlowAssetTypeAction::GetTypeColor() const
{
	return FColor::Orange;
}

const FSlateBrush* FGameFlowAssetTypeAction::GetIconBrush(const FAssetData& InAssetData, const FName InClassName) const
{
	const ISlateStyle* Style = FSlateStyleRegistry::FindSlateStyle(FGameFlowEditorStyle::TypeName);
	return Style->GetBrush("GameFlow.Editor.Default.AssetIcon");
}

const FSlateBrush* FGameFlowAssetTypeAction::GetThumbnailBrush(const FAssetData& InAssetData,
	const FName InClassName) const
{
	const ISlateStyle* Style = FSlateStyleRegistry::FindSlateStyle(FGameFlowEditorStyle::TypeName);
	return Style->GetBrush("GameFlow.Editor.Default.AssetIcon");
}

void FGameFlowAssetTypeAction::OpenAssetEditor(const TArray<UObject*>& InObjects,
                                               TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	// Create and initialize game flow asset editor.
	const TSharedPtr<GameFlowAssetToolkit> Editor = MakeShared<GameFlowAssetToolkit>();
	Editor->InitEditor(InObjects);
}
