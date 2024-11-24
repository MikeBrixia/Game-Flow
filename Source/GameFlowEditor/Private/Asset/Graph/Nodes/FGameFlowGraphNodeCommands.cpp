#include "Asset/Graph/Nodes/FGameFlowGraphNodeCommands.h"
#include "Asset/GameFlowEditorStyleWidgetStyle.h"

#define LOCTEXT_NAMESPACE "FGameFlowGraphNodeCommands"

FGameFlowGraphNodeCommands::FGameFlowGraphNodeCommands(): TCommands("GameFlowGraphNodeCommands",
	LOCTEXT("GameFlowGraphNodeCommands", "Game Flow graph node actions"),
	NAME_None, FGameFlowEditorStyle::TypeName)
{
}

void FGameFlowGraphNodeCommands::RegisterCommands()
{
	// Node Commands.
	{
		FUICommandInfo::MakeCommandInfo(SharedThis(this), CopyNode, "CopyNode",
									LOCTEXT("CopyNodeCommand", "Copy"),
									LOCTEXT("CopyNodeCommandDescription", "Copy this node"),
									FSlateIcon(), EUserInterfaceActionType::Button, FInputChord());

		FUICommandInfo::MakeCommandInfo(SharedThis(this), PasteNode, "PasteNode",
									LOCTEXT("PasteNodeCommand", "Paste"),
									LOCTEXT("PasteNodeCommandDescription", "Paste node inside the graph"),
									FSlateIcon(), EUserInterfaceActionType::Button, FInputChord());
		
		FUICommandInfo::MakeCommandInfo(SharedThis(this), ReplaceNode, "ReplaceNode",
									LOCTEXT("ReplaceNodeCommand", "Replace"),
									LOCTEXT("ReplaceNodeCommandDescription", "Replace this node with another game flow node"),
									FSlateIcon(GetStyleSetName(), "GameFlow.Editor.Default.Node"), EUserInterfaceActionType::Button, FInputChord());

		FUICommandInfo::MakeCommandInfo(SharedThis(this), RemoveNode, "RemoveNode",
										LOCTEXT("ValidateNodeCommand", "Remove"),
										LOCTEXT("RemoveNodeCommandDescription", "Remove this node from graph"),
										FSlateIcon(GetStyleSetName(), "GameFlow.Editor.Default.RemoveNode"), EUserInterfaceActionType::Button, FInputChord());
		
		FUICommandInfo::MakeCommandInfo(SharedThis(this), ValidateNode, "ValidateNode",
										LOCTEXT("ValidateNodeCommand", "Validate"),
										LOCTEXT("ReplaceNodeCommandDescription", "Validate and fix node invalid data"),
										FSlateIcon(GetStyleSetName(), "GameFlow.Editor.Common.Adjust"), EUserInterfaceActionType::Button, FInputChord());
	}
    
	// Debug commands.
	{
		FUICommandInfo::MakeCommandInfo(SharedThis(this), AddBreakpoint, "Add breakpoint",
									 LOCTEXT("AddBreakpointCommand", "Add breakpoint"),
									 LOCTEXT("AddBreakpointCommandDescription", "Add breakpoint to this node"),
									 FSlateIcon(GetStyleSetName(), "GameFlow.Editor.Default.Add"), EUserInterfaceActionType::Button, FInputChord());

		FUICommandInfo::MakeCommandInfo(SharedThis(this), RemoveBreakpoint, "Remove breakpoint",
									LOCTEXT("RemoveBreakpointCommand", "Remove breakpoint"),
									LOCTEXT("RemoveBreakpointCommandDescription", "Remove breakpoint from this node"),
									FSlateIcon(GetStyleSetName(), "GameFlow.Editor.Default.BreakpointDelete"), EUserInterfaceActionType::Button, FInputChord());
		
		FUICommandInfo::MakeCommandInfo(SharedThis(this), EnableBreakpoint, "Enable breakpoint",
									LOCTEXT("EnableBreakpointCommand", "Enable breakpoint"),
									LOCTEXT("EnableBreakpointCommandDescription", "Activate a breakpoint on this node"),
									FSlateIcon(GetStyleSetName(), "GameFlow.Editor.Debug.Icon.EnabledBreakpoint"), EUserInterfaceActionType::Button, FInputChord());
		
		FUICommandInfo::MakeCommandInfo(SharedThis(this), DisableBreakpoint, "Disable breakpoint",
										LOCTEXT("DisableBreakpointCommand", "Disable Breakpoint"),
										LOCTEXT("DisableBreakpointCommandDescription", "Remove a debug breakpoint on this node"),
										FSlateIcon(GetStyleSetName(), "GameFlow.Editor.Debug.Icon.DisabledBreakpoint"), EUserInterfaceActionType::Button, FInputChord());
	}

	// Utils.
	{
		FUICommandInfo::MakeCommandInfo(SharedThis(this), AddComment, "Add Comment",
										LOCTEXT("AddCommentCommand", "Add Comment"),
										LOCTEXT("AddCommandDescription", "Add comment bubble on top of the node"),
										FSlateIcon(), EUserInterfaceActionType::Button, FInputChord());
	}
}

#undef LOCTEXT_NAMESPACE