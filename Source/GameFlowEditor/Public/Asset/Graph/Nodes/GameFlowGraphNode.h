// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Config/FGameFlowNodeInfo.h"
#include "Nodes/GameFlowNode.h"
#include "Nodes/GameFlowNode_Dummy.h"
#include "Nodes/GameFlowNode_Input.h"
#include "UObject/Object.h"
#include "GameFlowGraphNode.generated.h"

class UGameFlowNode_Dummy;
DECLARE_MULTICAST_DELEGATE(FOnNodeAssetChanged)
DECLARE_MULTICAST_DELEGATE(FOnValidationEnd)

/**
 * A node used inside Game Flow graphs.
 */
UCLASS(NotBlueprintable, NotBlueprintType)
class GAMEFLOWEDITOR_API UGameFlowGraphNode : public UEdGraphNode
{
	friend class UGameFlowNodeFactory;
	friend struct FGameFlowNodeSchemaAction_NewNode;
	
	GENERATED_BODY()

public:
	/* Called when the node asset encapsulated inside this graph node gets changed. */
	FOnNodeAssetChanged OnNodeAssetChanged;
	/* Callback for when this node has finished being validated. */
	FOnValidationEnd OnValidationResult;
private:
	
	/* The game flow node asset encapsulated inside this graph node. */
	UPROPERTY()
	TObjectPtr<UGameFlowNode> NodeAsset;

	/* Node asset info red from global GameFlow plugin settings. */
	UPROPERTY()
	FGameFlowNodeInfo Info;

	/* True if the node asset is waiting to be compiled. */
	bool bPendingCompilation;
	
public:
	UGameFlowGraphNode();
	
	/**
	 * @brief Read Game Flow node asset and create graph node pins.
	 */
	void CreateNodePins(bool bAddToAsset = true);

	/**
	 * @brief Create a brand new pin for a Game Flow node.
	 * @param PinDirection The direction of the new pin.
	 * @param PinName The name of the new pin. Pins with name "None" will be ignored.
	 * @param bAddToAsset should we add this new pin to the Game Flow node asset? 
	 * @return The new pin.
	 */
	UEdGraphPin* CreateNodePin(const EEdGraphPinDirection PinDirection, FName PinName = EName::None, bool bAddToAsset = true);

	/**
	 * @brief Create default pins for this Game Flow node.
	 */
	virtual void AllocateDefaultPins() override;
	
	/**
	 * @brief Generate a unique pin name.
	 * @param SourcePinName The name of the previous pin, will be used
	 *                      to generate a new name with a predictable pattern.
	 * @return The generated pin name.
	 */
	virtual FName CreateUniquePinName(FName SourcePinName) const override;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	
	/**
	 * @brief Check if this graph node is considered as a root node(e.g. input node) or not.
	 * @return True if node is a root, false otherwise.
	 */
	FORCEINLINE bool IsRoot() const { return NodeAsset->IsA(UGameFlowNode_Input::StaticClass()); }
	bool IsOrphan() const;
	virtual bool CanUserDeleteNode() const override;
	
	void OnLiveOrHotReloadCompile();
	void OnAssetCompiled();
	void OnAssetBlueprintPreCompiled(UBlueprint* Blueprint);
	void OnAssetValidated();
	void OnAssetSelected(const FAssetData& AssetData);
	void OnDummyReplacement(UClass* ClassToReplace);
	
	virtual void ReconstructNode() override;
	void ReportError(EMessageSeverity::Type MessageSeverity);
	
	/* Get the asset contained inside this graph node. */
	FORCEINLINE UGameFlowNode* GetNodeAsset() const { return NodeAsset; }
    void SetNodeAsset(UGameFlowNode* Node);

	FORCEINLINE FGameFlowNodeInfo& GetNodeInfo() { return Info; }
	FORCEINLINE void SetNodeInfo(FGameFlowNodeInfo NewInfo) { this->Info = NewInfo; }
	FORCEINLINE virtual FLinearColor GetNodeTitleColor() const override
	{
		return Info.TitleBarColor;
	}

	FORCEINLINE static FEdGraphPinType GetGraphPinType()
	{
		FEdGraphPinType OutputPinInfo = {};
		OutputPinInfo.PinCategory = UEdGraphSchema_K2::PC_Exec;
		return OutputPinInfo;
	}

private:
	void InitNode();
	
	FORCEINLINE bool ShouldFilterAssetPicker(const FAssetData& AssetData) const
	{
		return AssetData.AssetClassPath == UGameFlowNode_Dummy::StaticClass()->GetClassPathName();
	}
};

