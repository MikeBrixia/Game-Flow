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
	FUICommandInfo::MakeCommandInfo(SharedThis(this), ValidateAsset, "Validate", INVTEXT("Validate"),
		                         INVTEXT("Fixup game flow asset data and refresh it"),
		                          FSlateIcon(GetStyleSetName(), "GameFlow.Editor.Common.Adjust"),
		                          EUserInterfaceActionType::Button, FInputChord());

	FUICommandInfo::MakeCommandInfo(SharedThis(this), DebugAsset, "Debug", INVTEXT("Debug"),
								 INVTEXT("Enable/disable debug mode for this asset"),
								  FSlateIcon(GetStyleSetName(), "GameFlow.Editor.Debug"),
								  EUserInterfaceActionType::ToggleButton, FInputChord());

	FUICommandInfo::MakeCommandInfo(SharedThis(this), SelectAssetInstance, "World Instance", INVTEXT("World Instance"),
								 INVTEXT("Select a specific asset instance to inspect"),
								  FSlateIcon(), EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE