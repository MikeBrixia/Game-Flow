﻿#pragma once

/** Contains a list of Game Flow editor input commands. */
class FGameFlowEditorCommands : public TCommands<FGameFlowEditorCommands>
{
public:
	
	FGameFlowEditorCommands();
	
	TSharedPtr<FUICommandInfo> ValidateAsset;
	TSharedPtr<FUICommandInfo> DebugAsset;
	TSharedPtr<FUICommandInfo> SelectAssetInstance;
	
	/** Register Game Flow editor input commands. */
	virtual void RegisterCommands() override;
};

