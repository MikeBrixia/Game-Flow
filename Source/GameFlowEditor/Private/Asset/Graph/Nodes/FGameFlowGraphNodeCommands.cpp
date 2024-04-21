#include "Asset/Graph/Nodes/FGameFlowGraphNodeCommands.h"
#include "Asset/GameFlowEditorStyleWidgetStyle.h"


FGameFlowGraphNodeCommands::FGameFlowGraphNodeCommands(): TCommands("GameFlowGraphNodeCommands",
	NSLOCTEXT("GameFlowEditor", "GameFlowGraphNodeCommands", "Game Flow graph node actions"),
	NAME_None, FGameFlowEditorStyle::TypeName)
{
	
}

void FGameFlowGraphNodeCommands::RegisterCommands()
{
	// Default node commands
	{
		FUICommandInfo::MakeCommandInfo(SharedThis(this), ReplaceNode, "ReplaceNode",
									NSLOCTEXT("GameFlowGraphNodeCommands", "ReplaceNodeCommand", "Replace"),
									NSLOCTEXT("GameFlowCommands", "ReplaceNodeCommandDescription", "Replace this node with another game flow node"),
									FSlateIcon(), EUserInterfaceActionType::Button, FInputChord());

		FUICommandInfo::MakeCommandInfo(SharedThis(this), ValidateNode, "ValidateNode",
										NSLOCTEXT("GameFlowGraphNodeCommands", "ValidateNodeCommand", "Validate"),
										NSLOCTEXT("GameFlowCommands", "ReplaceNodeCommandDescription", "Validate and fix node invalid data"),
										FSlateIcon(GetStyleSetName(), "GameFlow.Editor.Common.Adjust"), EUserInterfaceActionType::Button, FInputChord());
	}

	// Debug commands
	{
		FUICommandInfo::MakeCommandInfo(SharedThis(this), AddBreakpoint, "Add breakpoint",
									NSLOCTEXT("GameFlowGraphNodeCommands", "AddBreakpointCommand", "Add Breakpoint"),
									NSLOCTEXT("GameFlowCommands", "AddBreakpointCommandDescription", "Add a debug breakpoint on this node"),
									FSlateIcon(GetStyleSetName(), "GameFlow.Editor.Debug.Icon.EnabledBreakpoint"), EUserInterfaceActionType::Button, FInputChord());

		FUICommandInfo::MakeCommandInfo(SharedThis(this), RemoveBreakpoint, "Remove breakpoint",
										NSLOCTEXT("GameFlowGraphNodeCommands", "RemoveBreakpointCommand", "Remove Breakpoint"),
										NSLOCTEXT("GameFlowCommands", "RemoveBreakpointCommandDescription", "Remove a debug breakpoint on this node"),
										FSlateIcon(GetStyleSetName(), "GameFlow.Editor.Debug.Icon.DisabledBreakpoint"), EUserInterfaceActionType::Button, FInputChord());
	}
}
