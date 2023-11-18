// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFlowAsset.h"

UGameFlowAsset::UGameFlowAsset()
{
	// Initialize standard input.
	const FName StandardInputName = "Start";
	UGameFlowNode* StandardInputNode = NewObject<UGameFlowNode>(this, "GameFlowAsset");
	CustomInputs.Add(StandardInputName, StandardInputNode);

	// Initialize standard output
	const FName StandardOutputName = "Finish";
	UGameFlowNode* StandardOutputNode = NewObject<UGameFlowNode>(this, "GameFlowAsset");
	CustomOutputs.Add(StandardOutputName, StandardOutputNode);
}

void UGameFlowAsset::Execute(FName EntryPointName)
{
	UGameFlowNode* RootNode = CustomInputs.FindChecked(EntryPointName);
	if(RootNode != nullptr)
	{
		RootNode->Execute();
	}
}

void UGameFlowAsset::AddActiveNode(UGameFlowNode* Node)
{
	if(Node != nullptr)
	{
		this->ActiveNodes.Add(Node);
	}
}

void UGameFlowAsset::RemoveActiveNode(UGameFlowNode* Node)
{
	if(Node != nullptr)
	{
		ActiveNodes.Remove(Node);
	}
}

#if WITH_EDITOR

void UGameFlowAsset::ValidateAsset()
{
}

#endif