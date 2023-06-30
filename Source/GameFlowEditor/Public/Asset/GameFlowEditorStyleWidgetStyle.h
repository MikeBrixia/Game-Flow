// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateWidgetStyle.h"
#include "Styling/SlateWidgetStyleContainerBase.h"

#include "GameFlowEditorStyleWidgetStyle.generated.h"

/**
 * 
 */
USTRUCT()
struct GAMEFLOWEDITOR_API FGameFlowEditorStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FGameFlowEditorStyle();
	virtual ~FGameFlowEditorStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	inline static const FName TypeName = "GameFlowEditorStyle";
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FGameFlowEditorStyle& GetDefault();
	
	/** Get the GameFlow node slate style. */
	ISlateStyle& GetStyle() const
	{
		return *StyleSet;
	}
	
private:

	/* Unique style set for game flow node stylesheet */
	TSharedPtr<FSlateStyleSet> StyleSet;
	
};

/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UGameFlowEditorStyleWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_BODY()

public:
	/** The actual data describing the widget appearance. */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FGameFlowEditorStyle WidgetStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&WidgetStyle);
	}
};
