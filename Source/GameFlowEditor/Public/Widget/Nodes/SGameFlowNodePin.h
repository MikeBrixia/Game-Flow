// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphPin.h"
#include "KismetPins/SGraphPinExec.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class GAMEFLOWEDITOR_API SGameFlowNodePin : public SGraphPinExec
{
public:
	SLATE_BEGIN_ARGS(SGameFlowNodePin)
		{
		  ExecPinColor(FLinearColor::White),
		  PinDiffColor(FLinearColor::White);
		}
	SLATE_ARGUMENT(FLinearColor, ExecPinColor)
	SLATE_ARGUMENT(FLinearColor, PinDiffColor)
	SLATE_END_ARGS()
	
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, UEdGraphPin* Pin);

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	void SetPinDisplayName(const FName& PinName);
};
