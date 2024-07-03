// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFlowAsset.h"

#include "GameFlowSubsystem.h"
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
	ActiveNodes.Empty();
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

void UGameFlowAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
}

#endif
