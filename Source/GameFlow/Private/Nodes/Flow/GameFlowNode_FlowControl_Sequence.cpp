// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/Flow/GameFlowNode_FlowControl_Sequence.h"

UGameFlowNode_FlowControl_Sequence::UGameFlowNode_FlowControl_Sequence()
{
	bCanAddOutputPin = true;
	TypeName = "Conditional";

	AddInputPin("Exec");
	AddOutputPin("0");
}

void UGameFlowNode_FlowControl_Sequence::Execute_Implementation(const FName PinName)
{
	Super::Execute_Implementation(PinName);

	// Execute all the output pins in order.
	for(const auto& KeyValuePair : Outputs)
	{
		UOutPinHandle* PinHandle = KeyValuePair.Value;
		PinHandle->TriggerPin();
	}
}
