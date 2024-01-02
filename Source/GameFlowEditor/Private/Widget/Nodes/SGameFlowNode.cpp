// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Nodes/SGameFlowNode.h"

#include "ClassViewerModule.h"
#include "GameFlowEditor.h"
#include "GameFlowAsset.h"
#include "GraphEditorSettings.h"
#include "ObjectTools.h"
#include "SGraphPanel.h"
#include "SlateOptMacros.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Widget/Nodes/SGameFlowNodePin.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGameFlowNode::Construct(const FArguments& InArgs)
{
	SGraphNode::Construct();
	
	// Initialize node widget arguments.
	this->GraphNode = InArgs._Node;
	this->TitleText = InArgs._TitleText;

	UGameFlowGraphNode* GameFlowGraphNode = CastChecked<UGameFlowGraphNode>(GraphNode);
	// Each a time the encapsulated node asset gets changed, refresh the node widget.
    GameFlowGraphNode->OnNodeAssetChanged.AddSP(this, &SGameFlowNode::UpdateGraphNode);
	// Each time the encapsulated node gets validated, setup error info.
	GameFlowGraphNode->OnValidationResult.AddSP(this, &SGameFlowNode::UpdateGraphNode);
	
	UGameFlowNode_Dummy* DummyNode = Cast<UGameFlowNode_Dummy>(GameFlowGraphNode->GetNodeAsset());
	if(DummyNode != nullptr)
	{
		DummyNode->OnReplaceDummyNodeRequest.AddSP(this, &SGameFlowNode::OnRequestDummyReplacement);
	}
	
	// Use UCLASS display name attribute value as node title.
	if(InlineEditableText != nullptr)
	{
		InlineEditableText->SetText(TitleText);
	}
	
	// Construct node by reading GraphNode data.
	UpdateGraphNode();
	SetupErrorReporting();
}

void SGameFlowNode::OnRequestDummyReplacement(UClass* ClassToReplace)
{
	if(GEditor != nullptr)
	{
		GEditor->ClearPreviewComponents();
		const TSharedRef<SWindow> ClassReplacementDialog = CreateNodeReplacementDialog();
		/** Show the package dialog window as a modal window */
		GEditor->EditorAddModalWindow(ClassReplacementDialog);
	}
}

TSharedRef<SWindow> SGameFlowNode::CreateNodeReplacementDialog()
{
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.DisplayMode = EClassViewerDisplayMode::TreeView;
	
	const TSharedRef<SWidget> ClassViewerWidget = ClassViewerModule.CreateClassViewer(Options, FOnClassPicked::CreateLambda([=](UClass* PickedClass)
	{
		NodeReplacementClass = PickedClass;
	}));

	const FVector2D DEFAULT_WINDOW_SIZE(600, 700);
	/** Create the window to host our package dialog widget */
	TSharedRef<SWindow> ClassReplacementPickerWindow = SNew(SWindow)
		.Title(FText::FromString("Choose replacement class"))
		.ClientSize(DEFAULT_WINDOW_SIZE);

	TSharedRef<SVerticalBox> VerticalBoxContainer = SNew(SVerticalBox)
	 + SVerticalBox::Slot()
	.FillHeight(500)
	.MaxHeight(500)
	[
		ClassViewerWidget
	]
	+ SVerticalBox::Slot()
	  .AutoHeight()
	  .HAlign(HAlign_Left)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SCheckBox)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(INVTEXT("Replace all"))
		]
	]
	+ SVerticalBox::Slot()
	.AutoHeight()
	[
		SNew(SButton)
		.Text(INVTEXT("Replace"))
		.HAlign(HAlign_Right)
		.TextFlowDirection(ETextFlowDirection::LeftToRight)
	];
	
	ClassReplacementPickerWindow->SetContent(VerticalBoxContainer);

	return ClassReplacementPickerWindow;
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
	AddButton_CreatePin(EGPD_Input);
	return FReply::Handled();
}

FReply SGameFlowNode::OnAddOutputPin()
{
	AddButton_CreatePin(EGPD_Output);
	return FReply::Handled();
}

void SGameFlowNode::AddButton_CreatePin(EEdGraphPinDirection PinDirection)
{
	// Create new pin for this node.
	UGameFlowGraphNode* GameFlowGraphNode = CastChecked<UGameFlowGraphNode>(GraphNode);
	UEdGraphPin* NewPin = GameFlowGraphNode->CreateNodePin(PinDirection);
	
	// Create new pin widget.
	CreateStandardPinWidget(NewPin);
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
	const EEdGraphPinDirection PinDirection = PinToAdd->GetDirection();
	
	if (PinDirection == EGPD_Input)
	{
		LastPinBoxSlotIndex = InputPins.Num();
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
	else if(PinDirection == EGPD_Output)
	{
		LastPinBoxSlotIndex = OutputPins.Num();
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

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
