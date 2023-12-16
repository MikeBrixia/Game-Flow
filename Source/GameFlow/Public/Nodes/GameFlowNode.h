// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Config/GameFlowSettings.h"
#include "GameFlowNode.generated.h"

#if WITH_EDITORONLY_DATA

DECLARE_MULTICAST_DELEGATE(FOnNodePinNameChange)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnNodeTypeChange, const FName&)

#endif

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

	/* All the possible outputs of this node. */
	TMap<FName, TPair<FName, UGameFlowNode*>> Outputs;
	/* Callback for when one of this node pins name gets changed. */
	FOnNodePinNameChange OnNodePinNameChange;
	/* Callback for when the node type gets changed. */
	FOnNodeTypeChange OnNodeTypeChange;
	
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

public:
	
	UGameFlowNode();
	
	/* Execute GameFlow blueprint. */
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow")
	void Execute(const FName& PinName);
	virtual void Execute_Implementation(const FName& PinName);
	
protected:
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow")
	void OnFinishExecute();
	virtual void OnFinishExecute_Implementation();
	
	/**
	 * @brief Call this function to trigger an output and execute the next node.
	 * @param OutputPin Name of the pin to which the next node has been mapped.
	 * @param bFinish If true, this node will be the only output and node will be unloaded.
	 */
	UFUNCTION(BlueprintCallable, Category="Game Flow")
	void FinishExecute(FName OutputPin, bool bFinish);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
public:
	
	FORCEINLINE virtual TPair<FName, UGameFlowNode*> GetNextNode(FName PinName) const { return Outputs.FindRef(PinName); }
	
	FORCEINLINE virtual TArray<TPair<FName, UGameFlowNode*>> GetChildren() const
	{
		TArray<TPair<FName, UGameFlowNode*>> Children;
		Outputs.GenerateValueArray(Children);
		return Children;
	}

#if WITH_EDITOR

	FORCEINLINE virtual TArray<FName>& GetInputPins() { return InputPins; }
	FORCEINLINE virtual TArray<FName>& GetOutputPins() { return OutputPins; }
	FORCEINLINE bool CanAddInputPin() const { return bCanAddInputPin; }
	FORCEINLINE bool CanAddOutputPin() const { return bCanAddOutputPin; }
	
	/**
	 * @brief Generate a brand new and node-unique name for a node pin added with an AddPinButton.
	 * @param PinDirection an integer representing pin direction: 0 is input, 1 is output and 2 is max;
	 *                     values greater then 3 are undefined and can be used to implement custom pin directions.
	 * @return The generated FName.
	 */
	virtual FName GenerateAddPinName(uint8 PinDirection);

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
	 * @param Output The node and pins we want to connect to.
	 */
	void AddOutput(const FName PinName, const TPair<FName, UGameFlowNode*> Output);

	/**
	 * @brief Disconnect this node from the other node connected through the supplied pin.
	 * @param PinName The name of the pin which holds the connection.
	 */
	void RemoveOutput(const FName PinName);

	/**
	 * @brief Get all the registered Game Flow node types
	 * @return An array of node types.
	 */
	UFUNCTION(CallInEditor)
    FORCEINLINE TArray<FName> GetNodeTypeOptions() const { return UGameFlowSettings::Get()->Options; }
#endif
};


