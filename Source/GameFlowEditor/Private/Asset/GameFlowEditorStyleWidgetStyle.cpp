﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/GameFlowEditorStyleWidgetStyle.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define IMAGE_BRUSH_SVG( RelativePath, ... ) FSlateVectorImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".svg")), __VA_ARGS__)

FGameFlowEditorStyle::FGameFlowEditorStyle()
{
	// Initialize style set pointer.
	StyleSet = MakeShared<FSlateStyleSet>(TypeName);
    
	// Set style set content root filepath.
	StyleSet->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate/"));

#if ENGINE_MAJOR_VERSION < 5
	// Unreal Engine 4 Debug features styling
	{
		// Breakpoints.
		StyleSet->Set("GameFlow.Editor.Debug.EnabledBreakpoint", new IMAGE_BRUSH(TEXT("Old/Kismet2/Breakpoint_Valid"),
				   CoreStyleConstants::Icon22x22));
		StyleSet->Set("GameFlow.Editor.Debug.DisabledBreakpoint", new IMAGE_BRUSH(TEXT("Old/Kismet2/Breakpoint_Disabled"),
						   CoreStyleConstants::Icon22x22));
	}

#elif ENGINE_MAJOR_VERSION >= 5
	// Common Unreal Engine 5 styling
	{
		StyleSet->Set("GameFlow.Editor.Common.Blueprint", new IMAGE_BRUSH_SVG(TEXT("Starship/Common/blueprint"),
								   CoreStyleConstants::Icon22x22));
		StyleSet->Set("GameFlow.Editor.Common.Apply", new IMAGE_BRUSH_SVG(TEXT("Starship/Common/Apply"),
								   CoreStyleConstants::Icon22x22));
		StyleSet->Set("GameFlow.Editor.Common.Adjust", new IMAGE_BRUSH_SVG(TEXT("Starship/Common/Adjust"),
								   CoreStyleConstants::Icon22x22));
	}
	
	// Unreal Engine 5 Debug features styling
	{
		// Breakpoints.
		StyleSet->Set("GameFlow.Editor.Debug.EnabledBreakpoint", new IMAGE_BRUSH_SVG(TEXT("Starship/Blueprints/Breakpoint_Valid"),
								   CoreStyleConstants::Icon22x22));
		StyleSet->Set("GameFlow.Editor.Debug.DisabledBreakpoint", new IMAGE_BRUSH_SVG(TEXT("Starship/Blueprints/Breakpoint_Disabled"),
								   CoreStyleConstants::Icon22x22));
		StyleSet->Set("GameFlow.Editor.Debug.InvalidBreakpoint", new IMAGE_BRUSH_SVG(TEXT("Starship/Blueprints/Breakpoint_Invalid"),
								   CoreStyleConstants::Icon22x22));
		StyleSet->Set("GameFlow.Editor.Debug", new IMAGE_BRUSH_SVG(TEXT("Starship/Common/Bug"),
								   CoreStyleConstants::Icon22x22));

		// Debugging tools styling
		StyleSet->Set("GameFlow.Editor.Debug.BlueprintDebugger", new IMAGE_BRUSH_SVG(TEXT("Starship/Blueprints/Common/BlueprintDebugger"),
								   CoreStyleConstants::Icon22x22));
	}
#endif

	// Engine version independent styling.
	{
		StyleSet->Set("GameFlow.Editor.Default.Add", new IMAGE_BRUSH(TEXT("Icons/icon_add_40x"),
								  CoreStyleConstants::Icon22x22));
		StyleSet->Set("GameFlow.Editor.Default.RemoveNode", new IMAGE_BRUSH(TEXT("Icons/Edit/icon_Edit_Delete_40x"),
								  CoreStyleConstants::Icon22x22));
		StyleSet->Set("GameFlow.Editor.Default.BreakpointDelete", new IMAGE_BRUSH(TEXT("Icons/GenericDelete_Hovered"),
								  CoreStyleConstants::Icon22x22));
		StyleSet->Set("GameFlow.Editor.Default.Node", new IMAGE_BRUSH(TEXT("Graph/Icons/Node"),
								  CoreStyleConstants::Icon22x22));
		StyleSet->Set("GameFlow.Editor.Debug.Icon.EnabledBreakpoint", new IMAGE_BRUSH(TEXT("Old/Kismet2/Breakpoint_Valid_Small"),
								  CoreStyleConstants::Icon22x22));
		StyleSet->Set("GameFlow.Editor.Debug.Icon.DisabledBreakpoint", new IMAGE_BRUSH(TEXT("Old/Kismet2/Breakpoint_Disabled_Small"),
								  CoreStyleConstants::Icon22x22));
	}
}

FGameFlowEditorStyle::~FGameFlowEditorStyle()
{
}

const FGameFlowEditorStyle& FGameFlowEditorStyle::GetDefault()
{
	static FGameFlowEditorStyle Default;
	return Default;
}

void FGameFlowEditorStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	// Add any brush resources here so that Slate can correctly atlas and reference them
}
