// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/Flow/GameFlowNode_FlowControl_DoN.h"

UGameFlowNode_FlowControl_DoN::UGameFlowNode_FlowControl_DoN()
{
	// Initialize input pins.
	AddInputPin("Enter");
	AddInputPin("Reset");
	
    // Initialize output pins.
	AddOutputPin("Exit");

	TypeName = "Conditional";
	N = 1;
	Count = 0;
}

void UGameFlowNode_FlowControl_DoN::Execute_Implementation(const FName PinName)
{
	Super::Execute_Implementation(PinName);
	
	// Reset the counter.
	if(PinName.IsEqual("Reset"))
	{
		Count = 0;
	}
	else if(Count <= N)
	{
		Count++;
		ExecuteOutputPin("Exit");
	}
	FinishExecute(true);
}

