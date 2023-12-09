#include "Utils/GameFlowEditorSubsystem.h"

#include "GameFlowEditor.h"

void UGameFlowEditorSubsystem::RegisterActiveEditor(GameFlowAssetToolkit* AssetEditor)
{
	if(AssetEditor != nullptr)
	{
		const FName AssetName = AssetEditor->GetAsset()->GetFName();
		ActiveEditors.Add(AssetName, AssetEditor);
	}
}

void UGameFlowEditorSubsystem::UnregisterActiveEditor(GameFlowAssetToolkit* AssetEditor)
{
	if(AssetEditor != nullptr)
	{
		const FName AssetName = AssetEditor->GetAsset()->GetFName();
		ActiveEditors.Remove(AssetName);
	}
}

GameFlowAssetToolkit* UGameFlowEditorSubsystem::GetActiveEditorByAssetName(FName AssetName) const
{
	return ActiveEditors.FindRef(AssetName);
}

