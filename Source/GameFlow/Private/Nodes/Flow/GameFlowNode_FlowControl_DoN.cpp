// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/Flow/GameFlowNode_FlowControl_DoN.h"

UGameFlowNode_FlowControl_DoN::UGameFlowNode_FlowControl_DoN()
{
#if WITH_EDITOR
	// Initialize input pins.
	AddInputPin_CDO("Enter");
	AddInputPin_CDO("Reset");
	
    // Initialize output pins.
	AddOutputPin_CDO("Exit");

	TypeName = "Conditional";
#endif
	
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
		TriggerOutputPin("Exit");
	}
	FinishExecute(true);
}

void UGameFlowNode_FlowControl_DoN::OnFinishExecute_Implementation()
{
	Super::OnFinishExecute_Implementation();

	// Execute default output pin.
	TriggerOutputPin("Out");
}

