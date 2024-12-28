// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "PinHandle.h"
#include "Pins/InputPinHandle.h"
#include "Pins/OutPinHandles.h"
#include "GameFlowNode.generated.h"

#if WITH_EDITOR

DECLARE_MULTICAST_DELEGATE(FOnAssetRedirected)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAssetErrorEvent, EMessageSeverity::Type, FString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAssetExecuted, UInputPinHandle*, PinHandle);

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
	friend class UGameFlowAsset;
	friend class UPinHandle;
	friend class UExecPinHandle;
	friend class UOutPinHandle;
	
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	
public:

	/** Used to uniquely identify a node asset inside the editor.
	 * ID is shared between instances of a game flow assets. */
	UPROPERTY(VisibleAnywhere, TextExportTransient)
	FGuid GUID;

	/** If set to true, this node will be debugged independently of the editor debug mode. */
	UPROPERTY(EditAnywhere, Category="Config")
	bool bForceDebugView;
	
	/** The last tracked position of the node inside the graph. */
	UPROPERTY()
	FVector2D GraphPosition;

	/** Store developer comment about this node. */
	UPROPERTY()
	FString SavedNodeComment;
	
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

	/** Use this delegate to notify the editor that the asset is being executed*/
	UPROPERTY(TextExportTransient)
	FOnAssetExecuted OnAssetExecuted;
	
protected:
	/** True if this node should have a variable amount of input pins. */
    UPROPERTY(EditDefaultsOnly, Category="Game Flow|I/O")
    bool bCanAddInputPin;

	/** True if this node should have a variable amount of input pins. */
	UPROPERTY(EditDefaultsOnly, Category="Game Flow|I/O")
	bool bCanAddOutputPin;
#endif

public:

	/** All the node input pins. */
	UPROPERTY(EditDefaultsOnly, Category="Game Flow|I/O")
	TMap<FName, UInputPinHandle*> Inputs;
	
	/** All node output pins. */
	UPROPERTY(EditDefaultsOnly, Category="Game Flow|I/O", meta=(DisplayAfter="Inputs"))
	TMap<FName, UOutPinHandle*> Outputs;
	
	UGameFlowNode();

	void TryExecute(FName PinName);
	
protected:
	
	/** Execute this node */
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow")
	FORCEINLINE void Execute(const FName PinName = "Exec");
	FORCEINLINE virtual void Execute_Implementation(const FName PinName) {}
	
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow")
	FORCEINLINE void OnFinishExecute();
	FORCEINLINE virtual void OnFinishExecute_Implementation() {}
    
	/**
	 * @brief Call this function to trigger an output and execute the next node.
	 * @param bFinish If true, this node will be the only output and node will be unloaded.
	 */
	UFUNCTION(BlueprintCallable, Category="Game Flow")
	FORCEINLINE void FinishExecute(bool bFinish);

	UFUNCTION(BlueprintCallable, Category="Game Flow")
	FORCEINLINE void TriggerOutputPin(FName PinName);
	
private:
	
// Editor-only functionality used by external editors to manipulate this node.
#if WITH_EDITOR
	
public:
	FORCEINLINE void AddInputPin(FName PinName);
	void RemoveInputPin(FName PinName);
	
	FORCEINLINE void AddOutputPin(FName PinName);
    void RemoveOutputPin(FName PinName);
	
	UPinHandle* GetPinByName(FName PinName, TEnumAsByte<EEdGraphPinDirection> Direction) const;
	TArray<FName> GetInputPinsNames() const;
	TArray<FName> GetOutputPinsNames() const;
	
	bool CanAddInputPin() const;
	bool CanAddOutputPin() const;

	/**
	 * Is this node currently running inside parent asset?
	 * @returns True if node is currently running, false otherwise.
	 */
	bool IsActiveNode() const;

	/** Returns a list of all types of nodes defined inside Project Setting at Plugins/GameFlow. */
	UFUNCTION(CallInEditor)
	TArray<FName> GetNodeTypeOptions() const;
    
	/** Defines the tint and icon path for the node. */
	virtual void GetNodeIconInfo(FString& Key, FLinearColor& Color) const;

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	
private:
    UOutPinHandle* CreateExecOutputPin(FName PinName);
    UInputPinHandle* CreateExecInputPin(FName PinName);
    
    FName GeneratePinName(FName PinName) const;

	/** Return user-specified debug info. Use this when you want to debug
	 * properties which are not stored as UPROPERTY on the node or to add more
	 * complex debug behaviors. */
    virtual FString GetCustomDebugInfo() const;
#endif
};





