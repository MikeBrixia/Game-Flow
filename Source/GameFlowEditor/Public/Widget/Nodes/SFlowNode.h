// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "Brushes/SlateColorBrush.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Node widget used inside Game Flow derived graphs.
 * Extend this class if you want to change the behavior
 * and look of a Game Flow node or create brand new ones.
 */
class GAMEFLOWEDITOR_API SGameFlowNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGameFlowNode)
			: _Node(nullptr),
			  _TitleBackgroundColor(FSlateColor(FLinearColor::Red)),
	          _BodyBackgroundColor(FSlateColor(FLinearColor::Black)),
	          _TitleText(INVTEXT("Dummy_Node"))
	{}
	    SLATE_ARGUMENT(UGameFlowGraphNode*, Node)
	    SLATE_ARGUMENT(FSlateColor, TitleBackgroundColor)
	    SLATE_ARGUMENT(FSlateColor, BodyBackgroundColor)
	    SLATE_ARGUMENT(FText, TitleText)
	SLATE_END_ARGS()

protected:
	/* The image used as a background color for the title area of the node.*/
	FSlateColor TitleBackgroundColorBrush;
	
	/* The image used as a background color for the body area of the node.*/
	FSlateColor BodyBackgroundColorBrush;

	/* The text displayed inside the node titlebar. */
	FText TitleText;
	
public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
protected:

	/* The widget which represents the node title area. */
	TSharedPtr<SBorder> TitleAreaWidget;

	/* Stylesheet which determines the look of the node. */
	const TSharedPtr<ISlateStyle> NodeStyle;
	
    /* The widget which represents the node body area.*/
	//TSharedRef<SBorder> BodyAreaWidget;

	/** Create the node GameFlow node widget. */
	virtual TSharedRef<SWidget> CreateNodeWidget();
	
	/** Create the widget which covers the title area of the node.
	 *  Override this method if you want it to create your own
	 *  custom node title area widget look.
	 */
    virtual TSharedRef<SWidget> CreateTitleWidget(TSharedPtr<SNodeTitle> NodeTitle) override;
	
	/** Create the widget which covers the body area of the node.
	 *  The body is the part where input and output pins are displayed.
	 *  Override this method if you want it to create your own
	 *  custom node body area widget look.
	 */
	virtual TSharedRef<SWidget> CreateNodeContentArea() override;
	
	virtual void CreateStandardPinWidget(UEdGraphPin* Pin) override;
	
	/** Get the slate brush of the flow node body. */
	FORCEINLINE virtual const FSlateBrush* GetNodeBodyBrush() const override
	{
		return FAppStyle::GetBrush("Graph.Node.Body");
	}
	
	virtual const ISlateStyle* GetNodeStyle() const
	{
		const ISlateStyle* Style = FSlateStyleRegistry::FindSlateStyle("Flow Node Style");
		checkf(Style, TEXT("GameFlow Node stylesheet was null, make sure that your style has been registered inside the Slate Style Registry"));
		return Style;
	}
	
	FORCEINLINE virtual FSlateColor GetTitleTextColor() const
	{
		return FSlateColor(FLinearColor::White);
	}
	
	FORCEINLINE virtual const FSlateBrush* GetNodeTitleBrush() const
	{
		return FAppStyle::GetBrush("Graph.Node.TitleGloss");
	}
	
	/* Update graph node look. */
	virtual void UpdateGraphNode() override;
};
