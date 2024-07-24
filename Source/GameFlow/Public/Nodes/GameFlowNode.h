// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "PinHandle.h"
#include "GameFlowNode.generated.h"

#if WITH_EDITOR

DECLARE_MULTICAST_DELEGATE(FOnAssetRedirected)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAssetErrorEvent, EMessageSeverity::Type, FString);

#endif

/** Base class for all Game Flow nodes. */
UCLASS(Abstract, Blueprintable, BlueprintType, Category="Default", ClassGroup=(GameFlow))
class GAMEFLOW_API UGameFlowNode : public UObject
{
	// Friending this classes allows the graph editor to freely manipulate this node asset.
	friend class UGameFlowGraphSchema;
	friend class UGameFlowNodeFactory;
	friend class UGameFlowGraphNode;
	friend class FGameFlowConnectionDrawingPolicy;
	
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	
public:

	/** Used to uniquely identify a node asset inside the editor.
	 * ID is shared between instances of a game flow assets. */
	UPROPERTY(VisibleAnywhere)
	FGuid GUID;
	
	/** All the node input pins. */
	UPROPERTY(VisibleAnywhere, Category="Game Flow|I/O")
	TMap<FName, FPinHandle> Inputs;
	
	/** The last tracked position of the node inside the graph. */
	UPROPERTY()
	FVector2D GraphPosition;

	/** The type of this node(Latent, Event ecc.) */
	UPROPERTY(EditDefaultsOnly, meta=(GetOptions = "GetNodeTypeOptions"))
	FName TypeName;
	
	/** 
	 * True if user has placed a breakpoint on this specific node, false otherwise.
	 * When true, execution of the game flow asset will be paused on this node and
	 * should be resumed manually by the user.
	 */
	UPROPERTY()
	bool bBreakpointEnabled;

	UPROPERTY()
	bool bIsActive;
	
	/** Called when this asset gets deleted and replaced or hot-reloaded(C++ compilation) */
	FOnAssetRedirected OnAssetRedirected;
	
	/** Use this delegate to notify error events on this node to all listeners. */
	FOnAssetErrorEvent OnErrorEvent;
	
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
	
	/** Execute this node */
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow")
	FORCEINLINE void Execute(const FName PinName = "Exec");
	FORCEINLINE virtual void Execute_Implementation(const FName PinName) {};

protected:
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow")
	FORCEINLINE void OnFinishExecute();
	FORCEINLINE virtual void OnFinishExecute_Implementation() {};
	
	/**
	 * @brief Call this function to trigger an output and execute the next node.
	 * @param bFinish If true, this node will be the only output and node will be unloaded.
	 */
	UFUNCTION(BlueprintCallable, Category="Game Flow")
	FORCEINLINE void FinishExecute(bool bFinish);

	UFUNCTION(BlueprintCallable, Category="Game Flow")
	FORCEINLINE void ExecuteOutputPin(FName PinName);
	
// Editor-only functionality used by external editors to manipulate this node.
#if WITH_EDITOR
	
public:
	FORCEINLINE void AddInputPin(FName PinName);
	void RemoveInputPin(FName PinName);
	FORCEINLINE void AddOutputPin(FName PinName);
    void RemoveOutputPin(FName PinName);
    void UpdatePinHandle(const FPinHandle& UpdatedPinHandle);
	
	FPinHandle GetPinByName(FName PinName, TEnumAsByte<EEdGraphPinDirection> Direction) const;
	TArray<FName> GetInputPinsNames() const;
	TArray<FName> GetOutputPinsNames() const;
	bool CanAddInputPin() const;
	bool CanAddOutputPin() const;

	/**
	 * Is this node currently running inside parent asset?
	 * @returns True if node is currently running, false otherwise.
	 */
	bool IsActiveNode() const;
	
protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif
	
// Editor-only functionality used to define and communicate node look to the Game Flow editor.
#if WITH_EDITOR
	/** Returns a list of all types of nodes defined inside Project Setting at Plugins/GameFlow. */
	UFUNCTION(CallInEditor)
	TArray<FName> GetNodeTypeOptions() const;

	/** Defines the tint and icon path for the node. */
	virtual void GetNodeIconInfo(FString& Key, FLinearColor& Color) const;
#endif
};





