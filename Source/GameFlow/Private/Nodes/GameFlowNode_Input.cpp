// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/GameFlowNode_Input.h"

UGameFlowNode_Input::UGameFlowNode_Input()
{
#if WITH_EDITOR
	TypeName = "Input";
	AddOutputPin("Out");
#endif
}

void UGameFlowNode_Input::Execute_Implementation(const FName PinName)
{
	Super::Execute_Implementation(PinName);

	TriggerOutputPin("Out");
}


