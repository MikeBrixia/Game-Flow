#pragma once

/** Contains a list of Game Flow editor input commands. */
class FGameFlowEditorCommands : public TCommands<FGameFlowEditorCommands>
{
public:
	
	FGameFlowEditorCommands();

	/** Game Flow asset compile input command info. */
	TSharedPtr<FUICommandInfo> CompileAsset;

	/** Register Game Flow editor input commands. */
	virtual void RegisterCommands() override;
};

