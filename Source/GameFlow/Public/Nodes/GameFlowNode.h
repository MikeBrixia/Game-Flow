// Fill out your copyright notice in the Description page of Project Settings.

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
	UPROPERTY(EditAnywhere, Category="Game Flow|I/O")
	TArray<FName> InputPins;
	
	/* All the possible output pins for this node. */
	UPROPERTY(EditAnywhere, Category="Game Flow|I/O")
    TArray<FName> OutputPins;
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
	
	FORCEINLINE virtual TArray<FName> GetInputPins() const { return InputPins; }
	FORCEINLINE virtual TArray<FName> GetOutputPins() const { return OutputPins; }
	FORCEINLINE virtual UGameFlowNode* GetNextNode(FName PinName) const { return nullptr; }

	//--------
	
#if WITH_EDITORONLY_DATA
	/**
	 * @brief Connect this node to another graph node.
	 * @param PinName The name of the output pin which connects this node, to the next.
	 * @param Output The node to connect to.
	 */
	virtual void AddOutput(const FName& PinName, UGameFlowNode* Output);

	/**
	 * @brief Disconnect this node from the other node connected through the supplied pin.
	 * @param PinName The name of the pin which holds the connection.
	 */
	virtual void RemoveOutput(const FName& PinName);

	/**
	 * @brief Get all the registered Game Flow node types
	 * @return An array of node types.
	 */
	UFUNCTION(CallInEditor)
    FORCEINLINE TArray<FName> GetNodeTypeOptions() const { return UGameFlowSettings::Get()->Options; }
#endif
	
protected:
	
	/**
	 * @brief Call this function to trigger an output and execute the next node.
	 * @param OutputPin Name of the pin to which the next node has been mapped.
	 * @param bFinish If true, this node will be the only output and node will be unloaded.
	 */
	UFUNCTION(BlueprintCallable, Category="Game Flow")
	void FinishExecute(FName OutputPin, bool bFinish);
};

