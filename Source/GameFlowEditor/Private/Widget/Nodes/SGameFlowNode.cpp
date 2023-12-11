// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Nodes/SGameFlowNode.h"
#include "GameFlowAsset.h"
#include "GraphEditorSettings.h"
#include "SGraphPanel.h"
#include "SlateOptMacros.h"
#include "Widget/Nodes/SGameFlowNodePin.h"
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
}

void SGameFlowNode::CreateInputSideAddButton(TSharedPtr<SVerticalBox> InputBox)
{
	const UGameFlowNode* NodeAsset = CastChecked<UGameFlowGraphNode>(GraphNode)->GetNodeAsset();
	
	// Add input pin button only if node asset allows it.
	if(NodeAsset != nullptr && NodeAsset->CanAddInputPin())
	{
		AddInputPinButton = StaticCastSharedRef<SButton>(AddPinButtonContent(INVTEXT("Add Pin"),
			INVTEXT("Add a new input pin to this node"), false));
		InputBox->AddSlot()
		 .AutoHeight()
		 .VAlign(VAlign_Center)
		 .Padding(FMargin(10.f, 4.f))
		[
			AddInputPinButton.ToSharedRef()
		];
		
		FOnClicked OnButtonClicked;
		OnButtonClicked.BindRaw(this, &SGameFlowNode::OnAddInputPin);
		AddInputPinButton->SetOnClicked(OnButtonClicked);
	}
}

void SGameFlowNode::CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox)
{
	const UGameFlowNode* NodeAsset = CastChecked<UGameFlowGraphNode>(GraphNode)->GetNodeAsset();
	
	// Add output pin button only if node asset allows it.
	if(NodeAsset != nullptr && NodeAsset->CanAddOutputPin())
	{
		AddOutputPinButton = StaticCastSharedRef<SButton>(AddPinButtonContent(INVTEXT("Add Pin"),
			INVTEXT("Add a new output pin to this node"), true));
		OutputBox->AddSlot()
		 .AutoHeight()
		 .VAlign(VAlign_Center)
		 .Padding(FMargin(10.f, 4.f))
		[
			AddOutputPinButton.ToSharedRef()
		];
		
		FOnClicked OnButtonClicked;
		OnButtonClicked.BindRaw(this, &SGameFlowNode::OnAddOutputPin);
		AddInputPinButton->SetOnClicked(OnButtonClicked);
	}
}

void SGameFlowNode::CreateStandardPinWidget(UEdGraphPin* Pin)
{
	// Create the node pin widget. by default GameFlow will create an
	// exec pin for the node.
	const TSharedRef<SGraphPin> PinWidget = SNew(SGameFlowNodePin, Pin)
	                                         .ExecPinColor(FLinearColor::White)
	                                         .PinDiffColor(FLinearColor::White);
	// Add the pin to this node.
	this->AddPin(PinWidget);
}

FReply SGameFlowNode::OnAddInputPin()
{
	CreateGameFlowWidgetPin(EGPD_Input);
	return FReply::Handled();
}

FReply SGameFlowNode::OnAddOutputPin()
{
	CreateGameFlowWidgetPin(EGPD_Output);
	return FReply::Handled();
}

void SGameFlowNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));
	
	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = (PinObj != nullptr) && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility( TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced) );
	}

	int LastPinBoxSlotIndex;
	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		LastPinBoxSlotIndex = LeftNodeBox->NumSlots();
		LeftNodeBox->InsertSlot(LastPinBoxSlotIndex)
			.AutoHeight()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.Padding(Settings->GetInputPinPadding())
		[
			PinToAdd
		];
		InputPins.Add(PinToAdd);
	}
	else // Direction == EEdGraphPinDirection::EGPD_Output
		{
		LastPinBoxSlotIndex = RightNodeBox->NumSlots();
		RightNodeBox->InsertSlot(LastPinBoxSlotIndex)
			.AutoHeight()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.Padding(Settings->GetOutputPinPadding())
		[
			PinToAdd
		];
		OutputPins.Add(PinToAdd);
		}
}

void SGameFlowNode::CreateGameFlowWidgetPin(EEdGraphPinDirection PinDirection)
{
	UGameFlowGraphNode* GameFlowGraphNode = CastChecked<UGameFlowGraphNode>(GraphNode);
	UEdGraphPin* NewPin = GameFlowGraphNode->CreateNodePin(PinDirection);
	CreateStandardPinWidget(NewPin);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
