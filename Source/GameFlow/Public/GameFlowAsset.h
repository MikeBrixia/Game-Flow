// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Nodes/GameFlowNode.h"
#include "Nodes/GameFlowNode_Input.h"
#include "Nodes/GameFlowNode_Output.h"
#include "GameFlowAsset.generated.h"

/**
 * Game Flow asset, stores events flow in a tree
 * data structures.
 */
UCLASS(Blueprintable, BlueprintType)
class GAMEFLOW_API UGameFlowAsset : public UObject
{
	friend class UGameFlowGraphSchema;
	friend class GameFlowAssetToolkit;
	
	GENERATED_BODY()

public:

#if WITH_EDITORONLY_DATA
	/* All asset nodes mapped by their corresponding guid.*/
	UPROPERTY()
	TMap<uint32, UGameFlowNode*> Nodes;
	
	/* True if this asset has already been opened inside a GameFlow editor, false otherwise. */
	UPROPERTY()
	bool bHasAlreadyBeenOpened;
#endif
	
	/* All the user-defined entry points of the asset. */
	UPROPERTY(VisibleDefaultsOnly, Category="Game Flow")
	TMap<FName, UGameFlowNode_Input*> CustomInputs;
	
	/* All the user-defined exit points of the asset. */
	UPROPERTY(VisibleDefaultsOnly, Category="Game Flow")
	TMap<FName, UGameFlowNode_Output*> CustomOutputs;
	
private:
	
	/* The nodes currently being executed. */
	UPROPERTY(BlueprintGetter="GetActiveNodes")
	TArray<UGameFlowNode*> ActiveNodes;

#if WITH_EDITORONLY_DATA
	/* Nodes which are not connected to any parent. */
    UPROPERTY(VisibleDefaultsOnly)
    TArray<UGameFlowNode*> OrphanNodes;

	/* If true, asset will first be compiled and then saved. */
	UPROPERTY()
	bool bCompileOnSave;

	/* If true, asset will be compiled each time the users creates or breaks a connection. */
	UPROPERTY()
	bool bLiveCompile;

#endif
	
public:
	
	UGameFlowAsset();

	/**
	 * @brief Execute the asset from a selected entry point.
	 * @param EntryPointName The selected entry point.
	 */
	UFUNCTION(BlueprintCallable, Category="Game Flow")
	void Execute(FName EntryPointName);
	
	/**
	 * @brief Get the nodes which are currently being executed
	 *        by the Game Flow asset.
	 * @return The currently executed node
	 */
	UFUNCTION(BlueprintGetter, Category="Game Flow")
	FORCEINLINE TArray<UGameFlowNode*> GetActiveNodes() const
	{
		return ActiveNodes;
	}

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

#if WITH_EDITOR
    void ValidateAsset();
#endif
};
