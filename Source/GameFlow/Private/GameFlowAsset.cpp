// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFlowAsset.h"
#include "Nodes/GameFlowNode_Input.h"

UGameFlowAsset::UGameFlowAsset()
{
	this->bHasAlreadyBeenOpened = false;
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
}

UGameFlowNode_Input* UGameFlowAsset::CreateDefaultStartNode()
{
	// Initialize standard input.
	const FName StandardInputName = "Start";
	UGameFlowNode_Input* StandardInputNode = NewObject<UGameFlowNode_Input>(GetOuter(), "GameFlowAsset.StartNode");
	CustomInputs.Add(StandardInputName, StandardInputNode);
	Nodes.Add(StandardInputNode->GetUniqueID(), StandardInputNode);
	return StandardInputNode;
}

UGameFlowNode_Output* UGameFlowAsset::CreateDefaultFinishNode()
{
	// Initialize standard output
	const FName StandardOutputName = "Finish";
	UGameFlowNode_Output* StandardOutputNode = NewObject<UGameFlowNode_Output>(GetOuter(), "GameFlowAsset.FinishNode");
	CustomOutputs.Add(StandardOutputName, StandardOutputNode);
	Nodes.Add(StandardOutputNode->GetUniqueID(), StandardOutputNode);
	return StandardOutputNode;
}

#endif
