#pragma once

class FGameFlowGraphNodeCommands : public TCommands<FGameFlowGraphNodeCommands>
{

public:

	FGameFlowGraphNodeCommands();
	virtual void RegisterCommands() override;

	// Node commands.
	TSharedPtr<FUICommandInfo> ReplaceNode;
	TSharedPtr<FUICommandInfo> ValidateNode;
    TSharedPtr<FUICommandInfo> RemoveNode;
	
	// Debug commands.
	TSharedPtr<FUICommandInfo> AddBreakpoint;
	TSharedPtr<FUICommandInfo> EnableBreakpoint;
	TSharedPtr<FUICommandInfo> RemoveBreakpoint;
	TSharedPtr<FUICommandInfo> DisableBreakpoint;

	// Utils
	TSharedPtr<FUICommandInfo> AddComment;
};
