#include "Asset/GameFlowAssetTypeAction.h"
#include "Flow.h"
#include "GameFlowEditor.h"
#include "Asset/GameFlowAssetToolkit.h"

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
	return UFlow::StaticClass();
}

FColor FGameFlowAssetTypeAction::GetTypeColor() const
{
	return FColor::Orange;
}

void FGameFlowAssetTypeAction::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	MakeShared<GameFlowAssetToolkit>()->InitializeEditor(InObjects);
}
