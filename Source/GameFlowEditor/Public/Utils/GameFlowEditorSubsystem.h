#pragma once

#include "Asset/GameFlowAssetToolkit.h"
#include "GameFlowEditorSubsystem.generated.h"

/** This subsystem is the Game Flow editor manager. Important
 *  unique infos will be stored here.
 */
UCLASS()
class UGameFlowEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
public:
	
	/** The active game flow assets editors mapped by their
	 *  asset name. */
	TMap<FName, GameFlowAssetToolkit*> Editors;

	/** Register the given asset editor as an active editor.*/
	void RegisterActiveEditor(GameFlowAssetToolkit* AssetEditor);

	/** Unregister the given asset editor from the active editors.*/
	void UnregisterActiveEditor(GameFlowAssetToolkit* AssetEditor);
};
