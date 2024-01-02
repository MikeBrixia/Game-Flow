// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowNode_Dummy.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnReplaceDummyNodeRequest, UClass*)

/**
 * Dummy nodes are used as a replacement for invalid nodes
 * inside Game Flow Assets. Their goal is to replace the nodes
 * which have been marked as invalid and removed from the asset
 * while also keep the execution flow work.
 */
UCLASS(DisplayName="Dummy Node")
class GAMEFLOW_API UGameFlowNode_Dummy : public UGameFlowNode
{
	friend class UGameFlowGraphSchema;
	
	GENERATED_BODY()

public:
	
	/* Callback for when the developer requests a dummy node replacement. */
	FOnReplaceDummyNodeRequest OnReplaceDummyNodeRequest;

	UGameFlowNode_Dummy();
	virtual void Execute_Implementation(const FName& PinName) override;

private:

	// If true, all dummy nodes currently replacing the same ReplaceNodeClass will be
	// forced to replace the dummy.
	UPROPERTY(EditAnywhere, Category="Replace dummy|Function params")
	bool bReplaceAll;
	
	/* The class of the node this dummy has replaced. */
	UPROPERTY()
	UClass* ReplacedNodeClass;
	
	/* Replace the dummy node with another game flow node. */
	UFUNCTION(CallInEditor, Category="Replace dummy")
	void ReplaceDummyNode() const;
};
