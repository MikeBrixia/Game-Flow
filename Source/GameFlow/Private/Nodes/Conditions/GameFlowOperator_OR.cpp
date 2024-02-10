// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/Conditions/GameFlowOperator_OR.h"

UGameFlowOperator_OR::UGameFlowOperator_OR()
{
	TypeName = "Conditional";

	InputPins.Add("1");
	InputPins.Add("2");
    
	OutputPins.Add("Out");
	
	bCanAddInputPin = true;
}

void UGameFlowOperator_OR::Execute_Implementation(const FName& PinName)
{
	Super::Execute_Implementation(PinName);
}
