// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/Conditions/GameFlowNode_LogicalOperator_OR.h"

UGameFlowNode_LogicalOperator_OR::UGameFlowNode_LogicalOperator_OR()
{
	TypeName = "Conditional";

	InputPins.Add("1");
	InputPins.Add("2");
    
	OutputPins.Add("Out");
	
	bCanAddInputPin = true;
}

void UGameFlowNode_LogicalOperator_OR::Execute_Implementation(const FName& PinName)
{
	// OR logical operator just need to execute
	// it's output pin everytime it executes.
	FinishExecute("Out", true);
}
