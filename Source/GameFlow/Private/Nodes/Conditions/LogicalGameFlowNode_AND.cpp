// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/Conditions/LogicalGameFlowNode_AND.h"

ULogicalGameFlowNode_AND::ULogicalGameFlowNode_AND()
{
	// Initialize AND node as a conditional node.
	TypeName = "Conditional";

	// Initialize AND operator I/O default ports
	ConditionsPorts.Add(false);
	ConditionsPorts.Add(false);
	for(int i = 0; i < ConditionsPorts.Num(); i++)
	{
		const int PortNumber = i + 1;
		FName PortName = FName(FString::FromInt(PortNumber));
		// Initialize input pins.
		InputPins.Add(PortName);
	}
	bCanAddInputPin = true;

	// Initialize output pins.
	OutputPins.Add("Out");
}

void ULogicalGameFlowNode_AND::Execute_Implementation(const FName& PinName)
{
	Super::Execute_Implementation(PinName);
	
	bool bCanPass = false;
	// Check if all pins have been activated.
	for(const bool& ActivePort : ConditionsPorts)
	{
		bCanPass = ActivePort;
		if (!ActivePort) break;
	}

	// If all pins have been activated, finish execution
	// and trigger next node.
	if(bCanPass)
	{
		FinishExecute("Out", true);
	}
}

void ULogicalGameFlowNode_AND::OnFinishExecute_Implementation()
{
	// Reset node state after execution is ended. 
	// Check if all pins have been activated.
	for(int i = 0; i < ConditionsPorts.Num(); i++)
	{
		ConditionsPorts[i] = false;
	}
}

FName ULogicalGameFlowNode_AND::GenerateAddPinName(EEdGraphPinDirection PinDirection)
{
	FString Name;
	switch(PinDirection)
	{
	default: break;

	case EGPD_Input:
		Name = FString::FromInt(InputPins.Num() + 1);
		break;

	case EGPD_Output:
		Name = FString::FromInt(OutputPins.Num() + 1);
		break;
	}

	return FName(Name);
}
