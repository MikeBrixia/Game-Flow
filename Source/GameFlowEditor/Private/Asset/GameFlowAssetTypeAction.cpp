#include "Asset/GameFlowAssetTypeAction.h"
#include "Flow.h"
#include "GameFlowEditor.h"
#include "Asset/GameFlowAssetToolkit.h"
#include "Utils/GameFlowEditorSubsystem.h"

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
	// Create and initialize game flow asset editor.
	const TSharedPtr<GameFlowAssetToolkit> Editor = MakeShared<GameFlowAssetToolkit>();
	Editor->InitEditor(InObjects);
	
	// Is the global engine pointer valid?
	if(GEditor)
	{
		// If true, then register the new asset editor as an active editor inside game flow editor subsystem.
		UGameFlowEditorSubsystem* GameFlowSubsystem = GEditor->GetEditorSubsystem<UGameFlowEditorSubsystem>();
		GameFlowSubsystem->RegisterActiveEditor(Editor.Get());
	}
}
