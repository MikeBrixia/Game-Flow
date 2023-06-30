#include "Utils/GameFlowEditorSubsystem.h"

void UGameFlowEditorSubsystem::RegisterActiveEditor(GameFlowAssetToolkit* AssetEditor)
{
	if(AssetEditor != nullptr)
	{
		const FName AssetName = FName(AssetEditor->GetAsset()->GetName());
		Editors.Add(AssetName, AssetEditor);
	}
}

void UGameFlowEditorSubsystem::UnregisterActiveEditor(GameFlowAssetToolkit* AssetEditor)
{
	if(AssetEditor != nullptr)
	{
		const FName AssetName = FName(AssetEditor->GetAsset()->GetName());
		Editors.Remove(AssetName);
	}
}
