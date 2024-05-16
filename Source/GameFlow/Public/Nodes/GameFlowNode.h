// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PinHandle.h"
#include "Config/GameFlowSettings.h"
#include "GameFlowNode.generated.h"

/**
 * Serializable alternative to TPair for storing
 * Input pins name and nodes in Game Flow.
 */
USTRUCT(BlueprintType)
struct GAMEFLOW_API FGameFlowPinNodePair
{
	GENERATED_USTRUCT_BODY()
	
	FGameFlowPinNodePair();
	FGameFlowPinNodePair(const FName& InputPinName, UGameFlowNode* Node);

	UPROPERTY(EditAnywhere)
	FName InputPinName;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UGameFlowNode> Node;
};

DECLARE_MULTICAST_DELEGATE(FOnAssetRedirected)

/** Base class of all Game Flow nodes. */
UCLASS(Abstract, Blueprintable, BlueprintType, Category="Default", ClassGroup=(GameFlow))
class GAMEFLOW_API UGameFlowNode : public UObject
{
	friend class UGameFlowGraphSchema;
	friend class UGameFlowNodeFactory;
	friend class UGameFlowGraphNode;
	
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	
public:
	/** The last tracked position of the node inside the graph.*/
	UPROPERTY()
	FVector2D GraphPosition;

	/** The type of this node(Latent, Event ecc.)*/
	UPROPERTY(EditDefaultsOnly, meta=(GetOptions = "GetNodeTypeOptions"))
	FName TypeName;

	/** True if user has placed a breakpoint on this specific node, false otherwise.
	 * When true, execution of the game flow asset will be paused on this node and
	 * should be resumed manually by the user.
	 */
	bool bBreakpointEnabled;
	
	/** Called when this asset gets deleted and replaced or hot-reloaded(C++ compilation) */
	FOnAssetRedirected OnAssetRedirected;
	
protected:
	/** All the input pins of the node */
	UPROPERTY(EditDefaultsOnly, Category="Game Flow|I/O")
	TArray<FPinHandle> InputPins;
	
	/** All the output pins of the node */
	UPROPERTY(EditDefaultsOnly, Category="Game Flow|I/O")
    TArray<FPinHandle> OutputPins;

	/** True if this node should have a variable amount of input pins */
    UPROPERTY(EditDefaultsOnly, Category="Game Flow|I/O")
    bool bCanAddInputPin;

	/** True if this node should have a variable amount of input pins */
	UPROPERTY(EditDefaultsOnly, Category="Game Flow|I/O")
	bool bCanAddOutputPin;
#endif
	
private:

#if WITH_EDITORONLY_DATA
    UPROPERTY(VisibleAnywhere, Category="Game Flow|I/O")
    TMap<FName, FPinHandle> Inputs;
	
    TArray<FName> Temp_OldPinArray;
#endif
	
	/* All the possible outputs of this node. */
	UPROPERTY(VisibleAnywhere, Category="Game Flow|I/O")
	TMap<FName, FPinHandle> Outputs;
	
public:
	UGameFlowNode();
	
	/* Execute this node */
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow")
	void Execute(const FName& PinName = "Exec");
	virtual void Execute_Implementation(const FName& PinName);
	
	FORCEINLINE virtual FPinHandle GetNextNode(FName PinName) const { return Outputs.FindRef(PinName); }
	FORCEINLINE virtual TArray<FPinHandle> GetChildren() const
	{
		TArray<FPinHandle> Children;
		Outputs.GenerateValueArray(Children);
		return Children;
	}

protected:
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow")
	void OnFinishExecute();
	virtual void OnFinishExecute_Implementation();
	
	/**
	 * @brief Call this function to trigger an output and execute the next node.
	 * @param bFinish If true, this node will be the only output and node will be unloaded.
	 */
	UFUNCTION(BlueprintCallable, Category="Game Flow")
	void FinishExecute(bool bFinish);

	UFUNCTION(BlueprintCallable, Category="Game Flow")
	void ExecuteOutputPin(FName PinName);
	
// Editor-only functionality of game flow node.
#if WITH_EDITOR
	
public:
	FORCEINLINE TArray<FName>& GetInputPins() { return InputPins; }
	FORCEINLINE TArray<FName>& GetOutputPins() { return OutputPins; }
	FORCEINLINE bool CanAddInputPin() const { return bCanAddInputPin; }
	FORCEINLINE bool CanAddOutputPin() const { return bCanAddOutputPin; }

	void OnInputPinValueSet(FName PinName, int PinArrayIndex);
	void OnOutputPinValueSet(FName PinName, int PinArrayIndex);
	void OnPinRemoved(FName PinName);
	
	/**
	 * @brief Add a new output pin to this node.
	 * @param PinName The nome of the pin to create.
	 * @param Input The node and pin we want to connect to.
	 */
	void AddInputPin(const FName PinName, const FGameFlowPinNodePair Input);
	
	/**
	 * @brief Remove an input pin from this node.
	 * @param PinName The name of the pin to remove.
	 */
	void RemoveInputPin(const FName PinName);

	void ResetInputPins();
	void ResetOutputPins();
	
	/**
	 * @brief Connect this node to another graph node.
	 * @param PinName The name of the output pin which connects this node, to the next.
	 * @param Output The node and pin we want to connect to.
	 */
	void AddOutputPin(const FName PinName, const FGameFlowPinNodePair Output);

	/**
	 * @brief Disconnect this node from the other node connected through the supplied pin.
	 * @param PinName The name of the pin which holds the connection.
	 */
	void RemoveOutputPin(const FName PinName);
	
	/**
	 * @brief Get all the registered Game Flow node types
	 * @return An array of node types.
	 */
	UFUNCTION(CallInEditor)
    FORCEINLINE TArray<FName> GetNodeTypeOptions() { return UGameFlowSettings::Get()->Options; }
	
protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
   
private:
	/**
	 * Add a new logical input pin port.
	 * @remark Input ports are used to establish a connections between one of this
	 *         node input pins and another node output pins.
	 * @param PinName the name of the input pin to create the port from.
	 * @param Input the actual input port connection.
	 */
    void AddInputPort(const FName PinName, const FGameFlowPinNodePair Input);

	/**
	 * Add a new logical output pin port
	 * @remark Output ports are used to establish a connections between one of this
	 *         node output pins and another node input pins.
	 * @param PinName the name of the output pin to create the port from.
	 * @param Output the actual output port connection.
	 */
    void AddOutputPort(const FName PinName, const FGameFlowPinNodePair Output);

	/**
	 * Remove an active input pin port on this node.
	 * @param PinName The name of the pin.
	 */
	void RemoveInputPort(FName PinName);
	
	/**
	* Remove an active output port on this node.
	* @param PinName The name of the pin.
	*/
	void RemoveOutputPort(FName PinName);
#endif
};



