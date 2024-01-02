// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "KismetPins/SGraphPinExec.h"
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
	          _TitleText(INVTEXT("Dummy_Node"))
	{}
	    SLATE_ARGUMENT(UGameFlowGraphNode*, Node)
	    SLATE_ARGUMENT(FText, TitleText)
	SLATE_END_ARGS()

protected:
	/* The text displayed inside the node titlebar. */
	FText TitleText;

private:
    /* The class which will be used as a replacement for the current one. */
	UClass* NodeReplacementClass = nullptr;
	
public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	/* The widget which represents the node title area. */
	TSharedPtr<SBorder> TitleAreaWidget;

	void OnRequestDummyReplacement(UClass* ClassToReplace);
	virtual TSharedRef<SWindow> CreateNodeReplacementDialog();
	
	// -------------------- Widget pins --------------------------------
	
	TSharedPtr<SButton> AddInputPinButton;
	TSharedPtr<SButton> AddOutputPinButton;
	
	virtual void CreateInputSideAddButton(TSharedPtr<SVerticalBox> InputBox) override;
	virtual void CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox) override;
	virtual void CreateStandardPinWidget(UEdGraphPin* Pin) override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	
	/**
	 * @brief Called when an input pin gets created using the InputSideAddButton
	 * @return Event handler.
	 */
	virtual FReply OnAddInputPin();

	/**
	 * @brief Called when an output pin gets created using the InputSideAddButton
	 * @return Event handler.
	 */
	virtual FReply OnAddOutputPin();
	
private:
	void AddButton_CreatePin(EEdGraphPinDirection PinDirection);
	
	// ----------------- Widget styling ----------------------------------

	/* Stylesheet which determines the look of the node. */
	const TSharedPtr<ISlateStyle> NodeStyle;
	
	/** Get the slate brush of the GameFlow node body. */
	FORCEINLINE virtual const FSlateBrush* GetNodeBodyBrush() const override
	{
		return FAppStyle::GetBrush("Graph.Node.Body");
	}
	
	FORCEINLINE virtual const ISlateStyle* GetNodeStyle() const
	{
		const ISlateStyle* Style = FSlateStyleRegistry::FindSlateStyle("Flow Node Style");
		checkf(Style, TEXT("GameFlow Node stylesheet was null, make sure that your style has been registered inside the Slate Style Registry"));
		return Style;
	}

	// ---------------------------------------------------
};





