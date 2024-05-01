// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/Conditions/GameFlowNode_LogicalOperator_AND.h"
#include "Kismet/KismetStringLibrary.h"

UGameFlowNode_LogicalOperator_AND::UGameFlowNode_LogicalOperator_AND()
{
	// Initialize AND node as a conditional node.
	TypeName = "Conditional";

	// Initialize AND operator I/O default ports
	ConditionalPorts.Add(false);
	ConditionalPorts.Add(false);
	for(int i = 0; i < ConditionalPorts.Num(); i++)
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

void UGameFlowNode_LogicalOperator_AND::Execute_Implementation(const FName& PinName)
{
	const FString PinText = PinName.ToString();
	
	// Conditional AND node can only have 
	// numerical input pin ports.
	if(PinText.IsNumeric())
	{
		const int32 PinNameIndex = UKismetStringLibrary::Conv_StringToInt(PinName.ToString());
		const bool bPortValue = ConditionalPorts[PinNameIndex];
		// Is this port false(Has not been executed yet).
		if(!bPortValue)
		{
			// Update node conditional ports state.
			ConditionalPorts[PinNameIndex - 1] = true;
			TruePortsNum++;
		}
	}

	FinishExecute("Out", TruePortsNum == ConditionalPorts.Num());
}

void UGameFlowNode_LogicalOperator_AND::OnFinishExecute_Implementation()
{
	TruePortsNum = 0;
	// Reset node state after execution is ended. 
	// Check if all pins have been activated.
	for(int i = 0; i < ConditionalPorts.Num(); i++)
	{
		ConditionalPorts[i] = false;
	}
}
