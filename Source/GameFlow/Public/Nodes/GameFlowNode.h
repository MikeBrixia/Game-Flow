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

/**
 * Represents a single node in a Game Flow asset. This class forms the basis for all nodes in the Game Flow system,
 * ensuring a consistent interface for execution and pin management.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, Category="Default", ClassGroup=(GameFlow))
class GAMEFLOW_API UGameFlowNode : public UObject
{
	// Friending these classes allows the graph editor to freely manipulate this node asset.
	friend class UGameFlowGraphSchema;
	friend class UGameFlowNodeFactory;
	friend class UGameFlowGraphNode;
	friend class FGameFlowConnectionDrawingPolicy;
	friend class UGameFlowAsset;
	friend class UPinHandle;
	friend class UExecPinHandle;
	friend class UOutPinHandle;
	
	GENERATED_BODY()

public:

	/** Node input pins. */
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Game Flow|I/O")
	TMap<FName, UInputPinHandle*> Inputs;
	
	/** Node output pins. */
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Game Flow|I/O", meta=(DisplayAfter="Inputs"))
	TMap<FName, UOutPinHandle*> Outputs;
	
	UGameFlowNode();

	/**
	 * Attempts to execute the current game flow node associated with the specified input pin.
	 *
	 * @param PinName The name of the input pin that triggered this execution attempt.
	 */
	void TryExecute(FName PinName);
	
protected:
	/** Executes the node logic associated with the specified PinName.
	 *
	 * @param PinName The name of the pin to execute. Defaults to "Exec".
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow")
	FORCEINLINE void Execute(const FName PinName = "Exec");
	FORCEINLINE virtual void Execute_Implementation(const FName PinName) {}

	/**
	 * Triggers upon completion of the execution in a Game Flow node.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Game Flow")
	FORCEINLINE void OnFinishExecute();
	FORCEINLINE virtual void OnFinishExecute_Implementation() {}

	/**
	 * Completes the execution of a Game Flow node.
	 *
	 * @param bFinish Indicates whether the node has finished its execution.
	 */
	UFUNCTION(BlueprintCallable, Category="Game Flow")
	FORCEINLINE void FinishExecute(bool bFinish);

	/**
	 * Triggers the specified output pin by name, initiating any connected actions.
	 *
	 * @param PinName The name of the output pin to be triggered.
	 */
	UFUNCTION(BlueprintCallable, Category="Game Flow")
	FORCEINLINE void TriggerOutputPin(FName PinName);
	
	/** Returns a list of all types of nodes defined inside Project Setting at Plugins/GameFlow. */
	UFUNCTION(BlueprintGetter, CallInEditor)
	TArray<FName> GetNodeTypeOptions() const;
	
// Editor-only functionality used by external editors to manipulate this node.
#if WITH_EDITORONLY_DATA
	
public:

	/** Used to uniquely identify a node asset inside the editor.
	 * ID is shared between instances of game flow asset. */
	UPROPERTY(TextExportTransient)
	FGuid GUID;

	/** If set to true, this node will be debugged independently of the editor debug mode. */
	UPROPERTY(EditAnywhere, Category="Config")
	bool bForceDebugView;
	
	/** The last tracked position of the node inside the graph. */
	UPROPERTY()
	FVector2D GraphPosition;

	/** Developer comment about this node. */
	UPROPERTY()
	FString SavedNodeComment;

	UPROPERTY()
	bool bIsCommentBubbleActive;
	
	/** The type of this node (Latent, Event ecc.) */
	UPROPERTY(EditDefaultsOnly, meta=(GetOptions = "GetNodeTypeOptions"), Category="Config")
	FName TypeName;
	
	/** 
	 * True if the user has placed a breakpoint on this specific node, false otherwise.
	 * When true, execution of the game flow asset will be paused on this node and
	 * should be resumed manually by the user.
	 */
	UPROPERTY()
	bool bBreakpointPlaced;

	/**
	 * Indicates whether a breakpoint is currently active for debugging purposes.
	 * Used to determine if execution should pause when this node is reached.
	 */
	UPROPERTY()
	bool bBreakpointEnabled;
	
	/**
	 * Indicates whether the current object or component is active. This flag is used to control execution or visibility
	 * based on its state.
	 */
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
	/** True if this node should have a variable number of input pins. */
	UPROPERTY(EditDefaultsOnly, Category="Game Flow|I/O")
	bool bCanAddInputPin;

	/** True if this node should have a variable number of input pins. */
	UPROPERTY(EditDefaultsOnly, Category="Game Flow|I/O")
	bool bCanAddOutputPin;
	
public:
	/**
	 * Adds a pin to the current Game Flow node.
	 *
	 * @param PinName The name of the pin to be added.
	 * @param PinDirection The direction of the pin (input or output).
	 * @param PinType The type of the pin handle to be created (Exec, property...).
	 */
	FORCEINLINE void AddPin(FName PinName, EEdGraphPinDirection PinDirection,
		TSubclassOf<UPinHandle> PinType = nullptr, bool bCalledInsideConstructor = false);

	/**
	 * Removes a pin from the current Game Flow node by its name and direction.
	 *
	 * @param PinName The name of the pin to be removed.
	 * @param PinDirection The direction of the pin (input or output).
	 */
	FORCEINLINE void RemovePin(FName PinName, EEdGraphPinDirection PinDirection);

	/**
	 * Retrieves a pin handle by its name and pin direction.
	 *
	 * @param PinName The name of the pin to search for. Must not be None.
	 * @param Direction The direction of the pin (input or output) to filter the search.
	 * @return The UPinHandle found for the specified pin name and direction, or nullptr if not found.
	 */
	UPinHandle* GetPinByName(FName PinName, TEnumAsByte<EEdGraphPinDirection> Direction) const;

	/**
	 * Retrieves an array of pins associated with the specified direction.
	 * This can include input or output pins depending on the given direction.
	 *
	 * @param Direction The direction of the pins to retrieve (e.g., input or output).
	 * @return An array containing the pins of the specified direction.
	 */
	TArray<UPinHandle*> GetPinsByDirection(TEnumAsByte<EEdGraphPinDirection> Direction) const;
	TArray<FName> GetInputPinsNames() const;
	TArray<FName> GetOutputPinsNames() const;

	/**
	 * Computes the difference between the pins of this node and another node, given a specific direction.
	 *
	 * @param OtherNode The node to compare against.
	 * @param Diffs An array to store the details of any differences found.
	 * @param Direction The direction of the pins to be compared (input or output).
	 * @return The result of the comparison, encapsulated in an FDiffResults object.
	 */
	FDiffResults PinsDiff(const UGameFlowNode* OtherNode, TArray<FDiffSingleResult>& Diffs,
	                      EEdGraphPinDirection Direction) const;
	
	/**
	 * Determines whether an input pin can be added to this Game Flow node, using the "+" button.
	 *
	 * @return True if an input pin can be added, otherwise false.
	 */
	bool CanAddInputPin() const;

	/**
	 * Determines if an output pin can be added to this Game Flow node, using the "+" button.
	 *
	 * @return True if an output pin can be added; otherwise false.
	 */
	bool CanAddOutputPin() const;

	/**
	 * Is this node currently running inside the parent asset?
	 * @returns True if the node is currently running, false otherwise.
	 */
	bool IsActiveNode() const;
    
	/** Defines the tint and icon path for the node. */
	virtual void GetNodeIconInfo(FString& Key, FLinearColor& Color) const;
	
protected:
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	void AddInputPin_CDO(FName PinName);
	void AddOutputPin_CDO(FName PinName);
	
private:
	/**
	 * Retrieves custom debug information for the game flow node.
	 * Override this function if you want to add custom debug behavior.
	 * @return A string containing the custom debug information for the node.
	 */
	virtual FString GetCustomDebugInfo() const;
	
#endif
};





