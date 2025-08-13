// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFlowAsset.h"
#include "Nodes/GameFlowNode_Input.h"

UGameFlowAsset::UGameFlowAsset()
{
#if WITH_EDITOR
	this->bHasAlreadyBeenOpened = false;
#endif
}

void UGameFlowAsset::Execute(FName EntryPointName)
{
	UGameFlowNode* RootNode = CustomInputs.FindRef(EntryPointName);
	if(RootNode != nullptr)
	{
		RootNode->TryExecute("Exec");
	}
}

void UGameFlowAsset::TerminateExecution()
{
#if WITH_EDITOR
	// Terminate and clear all remaining active nodes.
	for (int i = ActiveNodes.Num() - 1; i >= 0; --i)
	{
		UGameFlowNode* Node = ActiveNodes[i];
		Node->OnFinishExecute();
		ActiveNodes.RemoveAt(i);
	}
#endif
	if(OnFinish.IsBound())
	{
		OnFinish.Broadcast(this);
	}
}

UGameFlowAsset* UGameFlowAsset::CreateInstance(UObject* Context)
{
	UGameFlowAsset* Instance = nullptr;
	if(Context != nullptr && IsAsset())
	{
		Instance = DuplicateObject(this, Context);
#if WITH_EDITOR
		Instance->TemplateAsset = this;
#endif
	}
	
	return Instance;
}

#if WITH_EDITOR

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
	return GUID.IsValid()? Nodes.FindRef(GUID) : nullptr;
}

#endif
