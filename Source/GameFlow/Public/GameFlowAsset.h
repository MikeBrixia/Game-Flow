// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Nodes/GameFlowNode.h"
#include "Nodes/GameFlowNode_Input.h"
#include "Nodes/GameFlowNode_Output.h"
#include "GameFlowAsset.generated.h"

class UGameFlowNode_FlowControl_Subgraph;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnFinish, UGameFlowAsset*)

/**
 * Game Flow asset is designed to help designer create their
 * own scripts to handle world and game events in a node-based
 * editor
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(GameFlow))
class GAMEFLOW_API UGameFlowAsset : public UObject
{
	friend class UGameFlowGraphSchema;
	friend class GameFlowAssetToolkit;
	friend class UGameFlowNode_Output;
	
	GENERATED_BODY()

public:
	/** If true, the game flow subsystem will not be allowed to create more than one instance of this asset.*/
	UPROPERTY(EditDefaultsOnly, Category="Config")
	bool bShouldBeSingleton;
	
	/** All the user-defined entry points of the asset. */
	UPROPERTY()
	TMap<FName, UGameFlowNode_Input*> CustomInputs;
	
	/** All the user-defined exit points of the asset. */
	UPROPERTY()
	TMap<FName, UGameFlowNode_Output*> CustomOutputs;
	
	/** Called when this asset finishes executing. */
	FOnFinish OnFinish;
	
	UGameFlowAsset();

	/**
	 * @brief Execute the asset from a selected entry point.
	 * @param EntryPointName The selected entry point.
	 */
	UFUNCTION(BlueprintCallable, Category="Game Flow", meta=(AdvancedDisplay="EntryPointName"))
	void Execute(FName EntryPointName = "Start");

	/**
	 * Create an instance from this game flow asset.
	 */
    UGameFlowAsset* CreateInstance(UObject* Context);
	
protected:
	
	/**
	* @brief Call this method when you need to terminate
	* the execution of this GameFlow object.
	*/
	void TerminateExecution();

#if WITH_EDITORONLY_DATA

public:
	/** All the node assets instanced by the user.*/
	UPROPERTY(VisibleAnywhere)
	TMap<FGuid, UGameFlowNode*> Nodes;
	
	/** True if this asset has already been opened inside a GameFlow editor, false otherwise. */
	UPROPERTY()
	bool bHasAlreadyBeenOpened;
	
	/** The source asset from which this node was duplicated. nullptr if this node is the source asset. */
	UPROPERTY()
	TSoftObjectPtr<UGameFlowAsset> TemplateAsset;
	
private:
	/* The nodes currently being executed. */
	UPROPERTY(DuplicateTransient, Transient)
	TArray<UGameFlowNode*> ActiveNodes;
	
public:
	/**
     * @brief Mark a game flow node as active(currently being executed).
     * @param Node The new current executed node.
     */
	void AddActiveNode(UGameFlowNode* Node);

	/**
	 * @brief Deactivate a node by removing it from the active nodes
	 *        list (Marking it as finished).
	 * @param Node The node to deactivate (Mark as finished).
	 */
	void RemoveActiveNode(UGameFlowNode* Node);
	
	/**
	 * @brief Get the nodes which are currently being executed
	 *        by the Game Flow asset.
	 * @remarks Editor-only.
	 * @return The currently executed node
	 */
	TArray<UGameFlowNode*> GetActiveNodes() const { return ActiveNodes; }

	/**
	 * Adds a node to the Game Flow asset by its globally unique identifier (GUID),
	 * ensuring it is valid.
	 *
	 * @param Node The node to be added to the Game Flow asset.
	 */
	void AddNode(UGameFlowNode* Node);

	/**
	 * Removes a node from the Game Flow asset, identified by its globally unique identifier (GUID).
	 * Ensures the node is removed only if it has a valid GUID.
	 *
	 * @param Node The node to be removed from the Game Flow asset.
	 */
	void RemoveNode(UGameFlowNode* Node);

	/**
	 * Retrieves all the nodes associated with the Game Flow asset.
	 * The nodes represent individual elements used in the node-based
	 * editor for handling world and game events.
	 *
	 * @return An array of pointers to the Game Flow nodes.
	 */
	TArray<UGameFlowNode*> GetNodes() const;
	
	/**
	 * Get a node by its globally unique identifier.
	 * @remarks Editor-only.
	 */
	UGameFlowNode* GetNodeByGUID(FGuid GUID) const;

#endif
};

