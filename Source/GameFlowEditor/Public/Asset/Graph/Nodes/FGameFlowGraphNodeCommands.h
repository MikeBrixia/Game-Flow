#pragma once

class FGameFlowGraphNodeCommands : public TCommands<FGameFlowGraphNodeCommands>
{

public:

	FGameFlowGraphNodeCommands();
	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> ReplaceNode;
	TSharedPtr<FUICommandInfo> ValidateNode;
};
