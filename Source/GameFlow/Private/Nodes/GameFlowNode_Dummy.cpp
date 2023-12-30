// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/GameFlowNode_Dummy.h"

UGameFlowNode_Dummy::UGameFlowNode_Dummy()
{
	this->bCanAddInputPin = true;
	this->bCanAddOutputPin = true;
}

void UGameFlowNode_Dummy::Execute_Implementation(const FName& PinName)
{
	Super::Execute_Implementation(PinName);
	FinishExecute(PinName, true);
}
