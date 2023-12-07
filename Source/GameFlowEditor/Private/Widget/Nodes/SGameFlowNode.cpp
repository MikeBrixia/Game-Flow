// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Nodes/SGameFlowNode.h"

#include "GameFlowAsset.h"
#include "GameFlowEditor.h"
#include "SlateOptMacros.h"
#include "KismetPins/SGraphPinExec.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGameFlowNode::Construct(const FArguments& InArgs)
{
	SGraphNode::Construct();

	// Initialize node widget arguments.
	this->GraphNode = InArgs._Node;
	this->TitleText = InArgs._TitleText;

	// Use UCLASS display name attribute value as node title.
	if(InlineEditableText != nullptr)
	{
		InlineEditableText->SetText(TitleText);
	}
	
	// Update the node(will be drawn if non-existing).
	UpdateGraphNode();

	UGameFlowNode* NodeAsset = InArgs._Node->GetNodeAsset();
	if(NodeAsset != nullptr)
	{
		if(NodeAsset->CanAddInputPin())
		{
			TSharedRef<SWidget> AddInputPinButton = AddPinButtonContent(INVTEXT("Add Pin"), INVTEXT("Add a new input pin to this node"), false);
			LeftNodeBox->AddSlot()
		     .AutoHeight()
		     .VAlign(VAlign_Center)    
		    [
			    AddInputPinButton
		    ];
		}
        
		if(NodeAsset->CanAddOutputPin())
		{
			TSharedRef<SWidget> AddOutputPinButton = AddPinButtonContent(INVTEXT("Add Pin"), INVTEXT("Add a new output pin to this node"), true);
			RightNodeBox->AddSlot()
			 .AutoHeight()
			 .VAlign(VAlign_Center)    
			[
				AddOutputPinButton
			];
		}
	}
}

void SGameFlowNode::CreateStandardPinWidget(UEdGraphPin* Pin)
{
	// Create the node pin widget. by default GameFlow will create an
	// exec pin for the node.
	const TSharedRef<SGraphPin> PinWidget = SNew(SGraphPinExec, Pin);
	
	// Make the pin image white.
	SImage* PinImage = static_cast<SImage*>(PinWidget->GetPinImageWidget().Get());
	PinImage->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	checkf(RightNodeBox, TEXT("Right node box is not valid!"));
	
	// Add the pin to this node.
	this->AddPin(PinWidget);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
