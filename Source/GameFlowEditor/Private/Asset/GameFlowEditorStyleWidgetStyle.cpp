// Fill out your copyright notice in the Description page of Project Settings.


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

	// Initialize style brushes...
	
// Unreal Engine 5 style.	
#if ENGINE_MAJOR_VERSION >= 5
	
	// Compile icons.
	StyleSet->Set("GameFlow.Editor.CompileIcon", new IMAGE_BRUSH_SVG(TEXT("Starship/Blueprints/CompileStatus_Background"),
	              CoreStyleConstants::Icon22x22));
	StyleSet->Set("GameFlow.Editor.CompileIcon.Fail", new IMAGE_BRUSH_SVG(TEXT("Starship/Blueprints/CompileStatus_Fail_Badge"),
				  CoreStyleConstants::Icon22x22));
	StyleSet->Set("GameFlow.Editor.CompileIcon.Success", new IMAGE_BRUSH_SVG(TEXT("Starship/Blueprints/CompileStatus_Good_Badge"),
				  CoreStyleConstants::Icon22x22));
	StyleSet->Set("GameFlow.Editor.CompileIcon.Warning", new IMAGE_BRUSH_SVG(TEXT("Starship/Blueprints/CompileStatus_Warning_Badge"),
				  CoreStyleConstants::Icon22x22));
	StyleSet->Set("GameFlow.Editor.CompileIcon.Unknown", new IMAGE_BRUSH_SVG(TEXT("Starship/Blueprints/CompileStatus_Unknown_Badge"),
				  CoreStyleConstants::Icon22x22));
	
// Unreal Engine 4 style
#elif ENGINE_MAJOR_VERSION == 4
    
#endif
	
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
