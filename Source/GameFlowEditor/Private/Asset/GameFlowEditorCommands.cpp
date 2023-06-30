#include "Asset/GameFlowEditorCommands.h"

#include "Asset/GameFlowEditorStyleWidgetStyle.h"

#define LOCTEXT_NAMESPACE "GameFlowGraphCommands"

FGameFlowEditorCommands::FGameFlowEditorCommands() : TCommands<FGameFlowEditorCommands>("GameFlowCommands",
	                                                                                   INVTEXT("Commands for the Game Flow editor"),
	                                                                                   NAME_None,FGameFlowEditorStyle::TypeName)
{
}

void FGameFlowEditorCommands::RegisterCommands()
{
	//UI_COMMAND(CompileAsset, "Compile", "Compile this Game Flow asset for use in game.", EUserInterfaceActionType::Button, FInputChord());
	FUICommandInfo::MakeCommandInfo(SharedThis(this), CompileAsset, "Compile", INVTEXT("Compile"),
		                         INVTEXT("Compile this Game Flow asset for use in game."),
		                          FSlateIcon(GetStyleSetName(), "GameFlow.Editor.CompileIcon"),
		                          EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE