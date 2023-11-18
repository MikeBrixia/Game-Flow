// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFlowNode.generated.h"

/* Base interface for all GameFlow nodes. */
UCLASS(Abstract, NotBlueprintable)
class GAMEFLOW_API UGameFlowNode : public UObject
{
	GENERATED_BODY()
 
    friend class UGameFlowNodeFactory;
	
private:

	/* Output children nodes mapped by their respective output pin name. */
	UPROPERTY()
	TMap<FName, UGameFlowNode*> Outputs;

public:

	UGameFlowNode();
	
	/* Execute GameFlow blueprint. */
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow")
	void Execute();
	virtual void Execute_Implementation();

protected:
	
	/**
	* @brief Call this function to trigger an output and execute the next node.
	 * @param OutputPin Name of the pin to which the next node has been mapped.
	 * @param Finish If true, this node will be the only output and node will be unloaded.
	 */
	UFUNCTION(BlueprintCallable, Category="Game Flow")
	void FinishExecute(FName OutputPin, bool Finish);
};

