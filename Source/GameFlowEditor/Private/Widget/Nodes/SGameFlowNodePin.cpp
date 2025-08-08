// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Nodes/SGameFlowNodePin.h"

#include "GameFlowEditor.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGameFlowNodePin::Construct(const FArguments& InArgs, UEdGraphPin* Pin)
{
	SGraphPinExec::Construct(SGraphPinExec::FArguments(), Pin);
	
	// Initialize pin properties.
	this->PinColorModifier = InArgs._ExecPinColor;
	this->PinDiffColor = InArgs._PinDiffColor;
	this->SetForegroundColor(InArgs._PinDiffColor);
	this->SetColorAndOpacity(InArgs._PinDiffColor);
	
	const TSharedRef<STextBlock> PinTextWidget = StaticCastSharedRef<STextBlock>(LabelAndValue->GetChildren()->GetChildAt(0));
	TAttribute<FText> PinTextAttribute;
	PinTextAttribute.BindRaw(GetPinObj(), &UEdGraphPin::GetDisplayName);
	PinTextWidget->SetText(PinTextAttribute);
	
	const TSharedPtr<SImage> PinImageCasted = StaticCastSharedPtr<SImage>(PinImage);
	PinImageCasted->SetColorAndOpacity(InArgs._ExecPinColor);
	
	const FString PinName = Pin->PinName.ToString();
	// Show only labels that do not contain the 'Exec' or 'out' Words.
	this->bShowLabel = !(PinName.Contains("Exec") || PinName.Contains("Out"));
}

void SGameFlowNodePin::SetPinDisplayName(const FName& PinName)
{
	GraphPinObj->PinFriendlyName = FText::FromName(PinName);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
