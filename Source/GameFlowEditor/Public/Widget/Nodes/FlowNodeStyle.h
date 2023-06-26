// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateWidgetStyle.h"
#include "Styling/SlateWidgetStyleContainerBase.h"
#include "FlowNodeStyle.generated.h"

/**
 * The default styling of all Game Flow nodes.
 */
USTRUCT()
struct GAMEFLOWEDITOR_API FFlowNodeStyle final : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

public:

	FFlowNodeStyle();
	virtual ~FFlowNodeStyle() override;
	
	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	virtual const FName GetTypeName() const override { return TypeName; }
	
	/** Get the GameFlow node slate style. */
	ISlateStyle& FFlowNodeStyle::GetStyle() const;
	
	/* Get Flow Node Style singleton instance. */
	static const FFlowNodeStyle& GetDefault();
	
	// Node properties
	static const FName Node_Body_Property_Name;
	static const FName Node_Title_Property_Name;

private:
	
	FName TypeName = "Flow Node Style";
	/* Unique style set for game flow node stylesheet */
	TSharedPtr<FSlateStyleSet> StyleSet;
    // Global singleton instance of Game Flow Node style.
	static TSharedPtr<FFlowNodeStyle> Instance;
};


// Provides a widget style container to allow us to edit properties in-editor
UCLASS(hidecategories = Object, MinimalAPI)
class UGameFlowNodeStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_UCLASS_BODY()

public:
	
	// This is our actual Style object. 
	UPROPERTY(EditAnywhere, Category = Appearance, meta = (ShowOnlyInnerProperties))
	FFlowNodeStyle GameFlowNodeStyle;

	// Retrieves the style that this container manages. 
	virtual const FSlateWidgetStyle* const GetStyle() const override
	{
		return &GameFlowNodeStyle;
	}

};