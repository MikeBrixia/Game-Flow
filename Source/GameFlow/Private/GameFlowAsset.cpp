﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFlowAsset.h"
#include "Nodes/GameFlowNode_Input.h"

UGameFlowAsset::UGameFlowAsset()
{
	this->bHasAlreadyBeenOpened = false;
}

void UGameFlowAsset::Execute(FName EntryPointName)
{
	UGameFlowNode* RootNode = CustomInputs.FindRef(EntryPointName);
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
#if WITH_EDITOR
	ActiveNodes.Empty();
#endif
	OnFinish.ExecuteIfBound(this);
}

UGameFlowAsset* UGameFlowAsset::CreateInstance(UObject* Context) const
{
	UGameFlowAsset* Instance = nullptr;
	if(Context != nullptr && IsAsset())
	{
		Instance = DuplicateObject(this, Context);
	}
	
	return Instance;
}

#if WITH_EDITOR

void UGameFlowAsset::AddNode(UGameFlowNode* Node)
{
	const FGuid GUID = Node->GUID;
	if(GUID.IsValid())
	{
		Nodes.Add(GUID, Node);
	}
}

void UGameFlowAsset::RemoveNode(UGameFlowNode* Node)
{
	const FGuid GUID = Node->GUID;
	if(GUID.IsValid())
	{
		Nodes.Remove(GUID);
	}
}

TArray<UGameFlowNode*> UGameFlowAsset::GetNodes() const
{
	TArray<UGameFlowNode*> NodeAssets;
	Nodes.GenerateValueArray(NodeAssets);
	return NodeAssets;
}

UGameFlowNode* UGameFlowAsset::GetNodeByGUID(FGuid GUID) const
{
	return GUID.IsValid()? Nodes.FindChecked(GUID) : nullptr;
}

void UGameFlowAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
}

#endif
