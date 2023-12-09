// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Nodes/SGameFlowNodePin.h"

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
	
	const TSharedPtr<SImage> PinImageCasted = StaticCastSharedPtr<SImage>(PinImage);
	PinImageCasted->SetColorAndOpacity(InArgs._ExecPinColor);
	
	FString PinName = Pin->PinName.ToString();
	// Initialize pin text.
	PinName = PinName.Contains("Exec") || PinName.Contains("Out")? "" : PinName;
	SetToolTipText(FText::FromString(PinName));
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
