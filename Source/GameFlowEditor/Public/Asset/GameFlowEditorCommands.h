﻿#pragma once

/** Contains a list of Game Flow editor input commands. */
class FGameFlowEditorCommands : public TCommands<FGameFlowEditorCommands>
{
public:
	
	FGameFlowEditorCommands();
	
	TSharedPtr<FUICommandInfo> CompileAsset;
    TSharedPtr<FUICommandInfo> CompileOnSave;
	TSharedPtr<FUICommandInfo> LiveCompile;
	
	/** Register Game Flow editor input commands. */
	virtual void RegisterCommands() override;
};

