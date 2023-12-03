// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/GameFlowNode_Input.h"

UGameFlowNode_Input::UGameFlowNode_Input()
{
	TypeName = "Input";
	OutputPins.Add("Out");
}

void UGameFlowNode_Input::AddOutput(const FName& PinName, UGameFlowNode* Output)
{
	// Are the output node and pin name valid?
	if(OutputNode != nullptr && !PinName.IsEqual("None"))
	{
		OutputNode = Output;
	}
}

void UGameFlowNode_Input::RemoveOutput(const FName& PinName)
{
	// If PinName is None, do not remove output.
	// this is just for convenience.
	if(!PinName.IsEqual("None"))
	{
		OutputNode = nullptr;
	}
}
