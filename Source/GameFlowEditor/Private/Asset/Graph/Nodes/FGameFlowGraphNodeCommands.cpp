#include "Asset/Graph/Nodes/FGameFlowGraphNodeCommands.h"
#include "Asset/GameFlowEditorStyleWidgetStyle.h"


FGameFlowGraphNodeCommands::FGameFlowGraphNodeCommands(): TCommands("GameFlowGraphNodeCommands",
	NSLOCTEXT("GameFlowEditor", "GameFlowGraphNodeCommands", "Game Flow graph node actions"),
	NAME_None, FGameFlowEditorStyle::TypeName)
{
	
}

void FGameFlowGraphNodeCommands::RegisterCommands()
{
	FUICommandInfo::MakeCommandInfo(SharedThis(this), ReplaceNode, "ReplaceNode",
		                            NSLOCTEXT("GameFlowGraphNodeCommands", "ReplaceNodeCommand", "Replace Node"),
		                            NSLOCTEXT("GameFlowCommands", "ReplaceNodeCommandDescription", "Replace this node with another game flow node"),
		                            FSlateIcon(), EUserInterfaceActionType::Button, FInputChord());
}
