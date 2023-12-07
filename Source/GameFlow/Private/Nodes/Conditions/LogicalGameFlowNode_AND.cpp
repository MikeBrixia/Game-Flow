// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/Conditions/LogicalGameFlowNode_AND.h"

ULogicalGameFlowNode_AND::ULogicalGameFlowNode_AND()
{
	// Initialize AND node as a conditional node.
	TypeName = "Conditional";
	
	bFirstPinTriggered = false;
	bSecondPinTriggered = false;
	
	// Initialize input pins.
	InputPins.Add("1");
	InputPins.Add("2");
	bCanAddInputPin = true;

	// Initialize output pins.
	OutputPins.Add("Out");
}

void ULogicalGameFlowNode_AND::Execute_Implementation(const FName& PinName)
{
	Super::Execute_Implementation(PinName);

	// Mark the pins has triggered.
	if(PinName == "1") bFirstPinTriggered = true;
	if(PinName == "2") bSecondPinTriggered = true;

	// If both pins have been triggered, trigger output pin.
	const bool bFinished = bFirstPinTriggered && bSecondPinTriggered;
	FinishExecute("Out", bFinished);
}

void ULogicalGameFlowNode_AND::OnFinishExecute_Implementation()
{
	// Reset node state after execution is ended. 
	bFirstPinTriggered = false;
	bSecondPinTriggered = false;
}
