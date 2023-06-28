// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Nodes/SFlowNode.h"
#include "SlateOptMacros.h"
#include "KismetPins/SGraphPinExec.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SFlowNode::Construct(const FArguments& InArgs)
{
	this->GraphNode = InArgs._Node;
	this->TitleBackgroundColorBrush = InArgs._TitleBackgroundColor;
	this->BodyBackgroundColorBrush = InArgs._BodyBackgroundColor;
	
	UpdateGraphNode();
}

TSharedRef<SWidget> SFlowNode::CreateNodeWidget()
{
	// Create node title widget and initialize it.
	const TSharedRef<SNodeTitle> NodeTitleWidget = SNew(SNodeTitle, GraphNode)
	.Text(FText::FromString("Generic Node Title"));
	
	const TSharedRef<SBorder> NodeWidget = SNew(SBorder)
	.BorderImage(FAppStyle::GetBrush("NoBorder"))
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SBox)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.MinDesiredWidth(80)
			.MinDesiredHeight(35)
			[
				SNew(SImage)
				.Image(GetNodeBodyBrush())
				.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 1.f)))
			]
		]
		+ SOverlay::Slot()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				CreateTitleWidget(NodeTitleWidget)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				CreateNodeContentArea()
			]
		]
    ];
	
	return NodeWidget;
}

TSharedRef<SWidget> SFlowNode::CreateTitleWidget(TSharedPtr<SNodeTitle> NodeTitle)
{
	return SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SBox)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.MinDesiredWidth(80)
			.MinDesiredHeight(25)
			[
				SNew(SImage)
				.Image(GetNodeTitleBrush())
				.ColorAndOpacity(GetTitleWidgetColor())
			]
		]
		+ SOverlay::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Start"))
				.Justification(ETextJustify::Left)
				.Margin(FMargin(10, 6,0,0))
				.TextStyle(FAppStyle::Get(), "Graph.Node.FTextBlockStyle")
				.ColorAndOpacity(GetTitleTextColor())
			]
		];
}

TSharedRef<SWidget> SFlowNode::CreateNodeContentArea()
{
	return SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SHorizontalBox)
			// Left inputs pin slot.
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(LeftNodeBox, SVerticalBox)
			]
			// Node center content slot.
			+ SHorizontalBox::Slot()
			.FillWidth(50.f)
			// Right inputs pin slot.
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(RightNodeBox, SVerticalBox)
			]
		];
	
}

void SFlowNode::CreateStandardPinWidget(UEdGraphPin* Pin)
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

void SFlowNode::UpdateGraphNode()
{
	// Reset input pins.
	InputPins.Empty();
	OutputPins.Empty();
	
	// Create node widget and add it to slate.
	const TSharedRef<SWidget> NodeWidget = CreateNodeWidget();
	GetOrAddSlot(ENodeZone::Center)[ NodeWidget ];

	// Create widgets for all node pins.
	// pins widgets MUST be created after the actual node widget.
	for(UEdGraphPin* Pin : GraphNode->Pins)
	{
		CreateStandardPinWidget(Pin);
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
