// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Nodes/FlowNodeStyle.h"
#include "Styling/SlateStyle.h"

TSharedPtr<FFlowNodeStyle> FFlowNodeStyle::Instance = nullptr;

// Initialize property name constants----------------------------------------------------.

const FName FFlowNodeStyle::Node_Body_Property_Name = "GameFlow.Node.Body";

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define IMAGE_BRUSH_SVG( RelativePath, ... ) FSlateVectorImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".svg")), __VA_ARGS__)

FFlowNodeStyle::FFlowNodeStyle()
{
	const FVector2D Icon16(16.0f, 16.0f);
	const FVector2D Icon20(20.0f, 20.0f);
	const FVector2D Icon30(30.0f, 30.0f);
	const FVector2D Icon40(40.0f, 40.0f);
	const FVector2D Icon64(64.0f, 64.0f);
	
	// Initialize style set pointer.
	StyleSet = MakeShared<FSlateStyleSet>(TypeName);
    
	// Set style set content root filepath.
	StyleSet->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate/"));
	// Add any brush resources here so that Slate can correctly atlas and reference them.
	StyleSet->Set("GameFlow.Node.Body", new BOX_BRUSH("Graph/RegularNode_body", Icon64));
	StyleSet->Set("GameFlow.Node.Body.Shadow", new BOX_BRUSH("Graph/RegularNode_DiffHighlight", FVector2D(1/64.f, 1/64.f)));
	// Define GameFlow node title style.
	StyleSet->Set("GameFlow.Node.Title", new BOX_BRUSH("Graph/RegularNode_title_gloss", Icon64));
	StyleSet->Set("GameFlow.Node.Title.Shadow", new BOX_BRUSH("Graph/RegularNode_title_highlight", Icon20));		
	
}

FFlowNodeStyle::~FFlowNodeStyle()
{
}

ISlateStyle& FFlowNodeStyle::GetStyle() const
{
	return *StyleSet;
}

const FFlowNodeStyle& FFlowNodeStyle::GetDefault()
{
	// Singleton instance init/get.
	if(!Instance.IsValid())
	{
		Instance = MakeShared<FFlowNodeStyle>();
	}
	return *Instance;
}

void FFlowNodeStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	// Add any brush resources here so that Slate can correctly atlas and reference them
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef IMAGE_BRUSH_SVG

// /////////////////////////////////// GAME FLOW NODE STYLE EDITOR OBJECT ////////////////////////////////////////////////////////

UGameFlowNodeStyle::UGameFlowNodeStyle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}
