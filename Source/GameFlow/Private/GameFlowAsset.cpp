// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFlowAsset.h"
#include "Nodes/GameFlowNode_Input.h"

UGameFlowAsset::UGameFlowAsset()
{
	this->bHasAlreadyBeenOpened = false;
	this->bCompileOnSave = true;
	this->bLiveCompile = true;
}

void UGameFlowAsset::Execute(FName EntryPointName)
{
	UGameFlowNode* RootNode = CustomInputs.FindChecked(EntryPointName);
	if(RootNode != nullptr)
	{
		RootNode->Execute("Exec");
	}
}

void UGameFlowAsset::AddActiveNode(UGameFlowNode* Node)
{
	if(Node != nullptr)
	{
		ActiveNodes.Add(Node);
	}
}

void UGameFlowAsset::RemoveActiveNode(UGameFlowNode* Node)
{
	if(Node != nullptr)
	{
		ActiveNodes.Remove(Node);
	}
}

void UGameFlowAsset::TerminateExecution()
{
	ActiveNodes.Empty();
}

#if WITH_EDITOR

void UGameFlowAsset::ValidateAsset()
{
	for(const auto& Pair : Nodes)
	{
		UGameFlowNode* Node = Pair.Value;
	}
}

#endif
