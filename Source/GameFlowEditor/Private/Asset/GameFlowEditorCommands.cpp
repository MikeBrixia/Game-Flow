#include "Asset/GameFlowEditorCommands.h"
#include "Asset/GameFlowEditorStyleWidgetStyle.h"

#define LOCTEXT_NAMESPACE "GameFlowGraphCommands"

FGameFlowEditorCommands::FGameFlowEditorCommands() : TCommands<FGameFlowEditorCommands>("GameFlowCommands",
	                                                                                   INVTEXT("Commands for the Game Flow editor"),
	                                                                                   NAME_None, FGameFlowEditorStyle::TypeName)
{
}

void FGameFlowEditorCommands::RegisterCommands()
{
	FUICommandInfo::MakeCommandInfo(SharedThis(this), CompileAsset, "Compile", INVTEXT("Compile"),
		                         INVTEXT("Compile this Game Flow asset for use in game."),
		                          FSlateIcon(GetStyleSetName(), "GameFlow.Editor.CompileIcon"),
		                          EUserInterfaceActionType::Button, FInputChord());

	FUICommandInfo::MakeCommandInfo(SharedThis(this), CompileOnSave, "CompileOnSave", INVTEXT("Compile on save"),
		                            INVTEXT("On each save, asset will be compiled"), FSlateIcon(), EUserInterfaceActionType::ToggleButton,
		                            FInputChord());

	FUICommandInfo::MakeCommandInfo(SharedThis(this), LiveCompile, "LiveCompile", INVTEXT("Live compile"),
									INVTEXT("Asset will be compiled on the fly each time a node link gets modified."), FSlateIcon(GetStyleSetName(), "GameFlow.Editor.CompileIcon"),
									EUserInterfaceActionType::ToggleButton, FInputChord());
}

#undef LOCTEXT_NAMESPACE