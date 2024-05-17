// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/Operators//GameFlowNode_LogicalOperator_AND.h"
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
		const FName PortName = FName(FString::FromInt(PortNumber));
		// Initialize input pins.
		AddInputPin(PortName);
	}
	bCanAddInputPin = true;
	TruePortsNum = 0;
	
	AddOutputPin("Out");
}

void UGameFlowNode_LogicalOperator_AND::Execute_Implementation(const FName& PinName)
{
	const FString PinText = PinName.ToString();
	
	// Conditional AND node can only have 
	// numerical input pin ports.
	if(PinText.IsNumeric())
	{
		const int32 PinIndex = UKismetStringLibrary::Conv_StringToInt(PinName.ToString()) - 1;
		const bool bPortValue = ConditionalPorts[PinIndex];
		// Is this port false(Has not been executed yet)?
		if(!bPortValue)
		{
			// Update node conditional ports state.
			ConditionalPorts[PinIndex] = true;
			TruePortsNum++;
		}
		
		if(TruePortsNum == ConditionalPorts.Num())
		{
			Reset();
			ExecuteOutputPin("Out");
		}
	}
	FinishExecute(true);
}

void UGameFlowNode_LogicalOperator_AND::Reset()
{
	// Reset node state after execution is ended. 
	TruePortsNum = 0;
	for (int i = 0; i < ConditionalPorts.Num(); i++)
	{
		ConditionalPorts[i] = false;
	}
}
