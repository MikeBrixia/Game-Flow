﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Config/GameFlowSettings.h"
#include "UObject/Interface.h"
#include "GameFlowNode.generated.h"

/* Base interface for all GameFlow nodes. */
UCLASS(Abstract, NotBlueprintable)
class GAMEFLOW_API UGameFlowNode : public UObject
{
	friend class UGameFlowNodeFactory;
	
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
public:
	/* The last tracked position of the node inside the graph.*/
	UPROPERTY()
	FVector2D GraphPosition;

	/* The type of this node(Latent, Event ecc.)*/
	UPROPERTY(EditAnywhere, meta=(GetOptions = "GetNodeTypeOptions"))
	FName TypeName;
	
protected:
	UPROPERTY(EditAnywhere, EditFixedSize, Category="Game Flow|I/O")
	TArray<FName> InputPins;
	
	/* All the possible output pins for this node. */
	UPROPERTY(EditAnywhere, EditFixedSize, Category="Game Flow|I/O")
    TArray<FName> OutputPins;

	/* True if user should be able to add more input pins than defaults by clicking on a '+' icon. */
    UPROPERTY(EditDefaultsOnly, Category="Game Flow|I/O")
    bool bCanAddInputPin;

	/* True if user should be able to add more output pins than defaults by clicking on a '+' icon. */
	UPROPERTY(EditDefaultsOnly, Category="Game Flow|I/O")
	bool bCanAddOutputPin;

#endif

private:

	/* All the possible outputs of this node. */
	UPROPERTY(VisibleDefaultsOnly, Category="Game Flow|I/O")
	TMap<FName, UGameFlowNode*> Outputs;

public:
	
	UGameFlowNode();
	
	/* Execute GameFlow blueprint. */
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow")
	void Execute(const FName& PinName);
	virtual void Execute_Implementation(const FName& PinName);

	UFUNCTION(BlueprintNativeEvent, Category="Game Flow")
	void OnFinishExecute();
	virtual void OnFinishExecute_Implementation();

	
	//-------- Functions with return types cannot be declared PURE_VIRTUAL(),
    //-------- for this reason we need to create empty return type functions.
	
	FORCEINLINE virtual TArray<FName>& GetInputPins() { return InputPins; }
	FORCEINLINE virtual TArray<FName>& GetOutputPins() { return OutputPins; }
	FORCEINLINE virtual UGameFlowNode* GetNextNode(FName PinName) const { return nullptr; }
    FORCEINLINE bool CanAddInputPin() const { return bCanAddInputPin; }
	FORCEINLINE bool CanAddOutputPin() const { return bCanAddOutputPin; }
	
	//--------

protected:
	/**
	 * @brief Call this function to trigger an output and execute the next node.
	 * @param OutputPin Name of the pin to which the next node has been mapped.
	 * @param bFinish If true, this node will be the only output and node will be unloaded.
	 */
	UFUNCTION(BlueprintCallable, Category="Game Flow")
	void FinishExecute(FName OutputPin, bool bFinish);

#if WITH_EDITORONLY_DATA

public:
	
	/**
	 * @brief Generate a brand new and node-unique name for a node pin added with an AddPinButton.
	 * @return The generated FName.
	 */
	UFUNCTION(BlueprintNativeEvent)
	virtual FName GenerateAddPinName(int PinDirection);
	
	/**
	 * @brief Add a new output pin to this node.
	 * @param PinName The nome of the pin to create.
	 */
	void AddInput(const FName PinName);

	/**
	 * @brief Remove an input pin from this node.
	 * @param PinName The name of the pin to remove.
	 */
	void RemoveInputPin(const FName PinName);
	
	/**
	 * @brief Connect this node to another graph node.
	 * @param PinName The name of the output pin which connects this node, to the next.
	 * @param Output The node to connect to.
	 */
	void AddOutput(const FName& PinName, UGameFlowNode* Output);

	/**
	 * @brief Disconnect this node from the other node connected through the supplied pin.
	 * @param PinName The name of the pin which holds the connection.
	 */
	void RemoveOutput(const FName& PinName);

	/**
	 * @brief Get all the registered Game Flow node types
	 * @return An array of node types.
	 */
	UFUNCTION(CallInEditor)
    FORCEINLINE TArray<FName> GetNodeTypeOptions() const { return UGameFlowSettings::Get()->Options; }
#endif
};

