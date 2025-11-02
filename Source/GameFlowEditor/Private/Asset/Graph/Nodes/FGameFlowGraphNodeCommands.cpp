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
		FUICommandInfo::MakeCommandInfo(SharedThis(this), ReplaceNode, "ReplaceNode",
									LOCTEXT("ReplaceNodeCommand", "Replace"),
									LOCTEXT("ReplaceNodeCommandDescription", "Replace this node with another game flow node"),
									FSlateIcon(GetStyleSetName(), "GameFlow.Editor.Default.Node"), EUserInterfaceActionType::Button, FInputChord());
		
		FUICommandInfo::MakeCommandInfo(SharedThis(this), ValidateNode, "ValidateNode",
										LOCTEXT("ValidateNodeCommand", "Validate"),
										LOCTEXT("ReplaceNodeCommandDescription", "Validate and fix node invalid data"),
										FSlateIcon(GetStyleSetName(), "GameFlow.Editor.Common.Adjust"), EUserInterfaceActionType::Button, FInputChord());

		FUICommandInfo::MakeCommandInfo(SharedThis(this), RemovePin, "RemovePin",
										LOCTEXT("RemovePinCommand", "Remove Pin"),
										LOCTEXT("RemovePinCommandDescription", "Remove this pin"),
										FSlateIcon(GetStyleSetName(), "GameFlow.Editor.Common.Remove"), EUserInterfaceActionType::Button, FInputChord());
	}
}

#undef LOCTEXT_NAMESPACE