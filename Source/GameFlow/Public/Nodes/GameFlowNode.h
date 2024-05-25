// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "PinHandle.h"
#include "Config/GameFlowSettings.h"
#include "GameFlowNode.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAssetRedirected)

/** Base class for all Game Flow nodes. */
UCLASS(Abstract, Blueprintable, BlueprintType, Category="Default", ClassGroup=(GameFlow))
class GAMEFLOW_API UGameFlowNode : public UObject
{
	friend class UGameFlowGraphSchema;
	friend class UGameFlowNodeFactory;
	friend class UGameFlowGraphNode;
	
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	
public:
	/** All the node input pins. */
	UPROPERTY(VisibleAnywhere, Category="Game Flow|I/O")
	TMap<FName, FPinHandle> Inputs;
	
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

	/** True if this node should have a variable amount of input pins */
    UPROPERTY(EditDefaultsOnly, Category="Game Flow|I/O")
    bool bCanAddInputPin;

	/** True if this node should have a variable amount of input pins */
	UPROPERTY(EditDefaultsOnly, Category="Game Flow|I/O")
	bool bCanAddOutputPin;
#endif

public:
	
	/** All node output pins. */
	UPROPERTY(VisibleAnywhere, Category="Game Flow|I/O", meta=(DisplayAfter="Inputs"))
	TMap<FName, FPinHandle> Outputs;
	
	UGameFlowNode();
	
	/* Execute this node */
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow")
	void Execute(const FName& PinName = "Exec");
	virtual void Execute_Implementation(const FName& PinName);

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
	
	/**
	 * @brief Get all the registered Game Flow node types
	 * @return An array of node types.
	 */
	UFUNCTION(CallInEditor)
    FORCEINLINE TArray<FName> GetNodeTypeOptions() { return UGameFlowSettings::Get()->Options; }

	void AddInputPin(FName PinName);
	void RemoveInputPin(FName PinName);
	void AddOutputPin(FName PinName);
    void RemoveOutputPin(FName PinName);
    void UpdatePinHandle(const FPinHandle& UpdatedPinHandle);
	
	FPinHandle GetPinByName(FName PinName, TEnumAsByte<EEdGraphPinDirection> Direction) const;
	TArray<FName> GetInputPinsNames() const;
	TArray<FName> GetOutputPinsNames() const;
	FORCEINLINE bool CanAddInputPin() const { return bCanAddInputPin; }
	FORCEINLINE bool CanAddOutputPin() const { return bCanAddOutputPin; }
	
protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};




