#pragma once
#include "Framework/Commands/Commands.h"

class FGameFlowGraphNodeCommands : public TCommands<FGameFlowGraphNodeCommands>
{

public:

	FGameFlowGraphNodeCommands();
	virtual void RegisterCommands() override;

	// Node commands.
	TSharedPtr<FUICommandInfo> ReplaceNode;
	TSharedPtr<FUICommandInfo> ValidateNode;
	TSharedPtr<FUICommandInfo> RemovePin;
};
