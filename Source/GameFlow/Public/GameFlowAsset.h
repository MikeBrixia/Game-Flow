// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Nodes/GameFlowNode.h"
#include "Nodes/GameFlowNode_Input.h"
#include "Nodes/GameFlowNode_Output.h"
#include "GameFlowAsset.generated.h"

class UGameFlowNode_FlowControl_Subgraph;
DECLARE_DELEGATE_OneParam(FOnFinish, UGameFlowAsset*)

/**
 * Game Flow InstancedAsset are designed to help designer create their
 * own scripts to handle world and game events in a node-based
 * editor
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(GameFlow))
class GAMEFLOW_API UGameFlowAsset : public UObject
{
	friend class UGameFlowGraphSchema;
	friend class GameFlowAssetToolkit;
	
	GENERATED_BODY()

public:

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere)
	TArray<UGameFlowNode*> Nodes;
	
	/** True if this asset has already been opened inside a GameFlow editor, false otherwise. */
	UPROPERTY()
	bool bHasAlreadyBeenOpened;
#endif

	/** If true, game flow subsystem will not be allowed to create more than one instance of this asset.*/
	UPROPERTY(EditDefaultsOnly, Category="Config")
	bool bShouldBeSingleton;
	
	/** All the user-defined entry points of the asset. */
	UPROPERTY(EditDefaultsOnly, EditFixedSize, Category="Game Flow")
	TMap<FName, UGameFlowNode_Input*> CustomInputs;
	
	/** All the user-defined exit points of the asset. */
	UPROPERTY(EditDefaultsOnly, EditFixedSize, Category="Game Flow")
	TMap<FName, UGameFlowNode_Output*> CustomOutputs;
	
	/** Called when this asset finishes executing. */
	FOnFinish OnFinish;
private:
	
	/* The nodes currently being executed. */
	UPROPERTY(BlueprintGetter="GetActiveNodes")
	TArray<UGameFlowNode*> ActiveNodes;

public:
	
	UGameFlowAsset();

	/**
	 * @brief Execute the asset from a selected entry point.
	 * @param EntryPointName The selected entry point.
	 */
	UFUNCTION(BlueprintCallable, Category="Game Flow", meta=(AdvancedDisplay="EntryPointName"))
	void Execute(FName EntryPointName = "Start");
	
	/**
	 * @brief Get the nodes which are currently being executed
	 *        by the Game Flow asset.
	 * @return The currently executed node
	 */
	UFUNCTION(BlueprintGetter, Category="Game Flow")
	FORCEINLINE TArray<UGameFlowNode*> GetActiveNodes() const { return ActiveNodes; }
	
	/**
	 * @brief Mark a game flow node as active(currently being executed).
	 * @param Node The new current executed node.
	 */
	void AddActiveNode(UGameFlowNode* Node);

	/**
	 * @brief Deactivate a node by removing it from the active nodes
	 *        list(Marking it as finished).
	 * @param Node The node to deactivate(Mark as finished).
	 */
	void RemoveActiveNode(UGameFlowNode* Node);
	
	/**
	 * @brief Call this method when you need to terminate
	 * the execution of this GameFlow object.
	 */
	void TerminateExecution();

	/**
	 * Create an instance from this game flow asset.
	 */
    UGameFlowAsset* CreateInstance(UObject* Context) const;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
