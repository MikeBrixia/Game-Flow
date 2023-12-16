#pragma once

/** Contains a list of Game Flow editor input commands. */
class FGameFlowEditorCommands : public TCommands<FGameFlowEditorCommands>
{
public:
	
	FGameFlowEditorCommands();

	
	TSharedPtr<FUICommandInfo> CompileAsset;

	// -------------------------- Pin commands ------------------------------
	
	/** Register Game Flow editor input commands. */
	virtual void RegisterCommands() override;
};

