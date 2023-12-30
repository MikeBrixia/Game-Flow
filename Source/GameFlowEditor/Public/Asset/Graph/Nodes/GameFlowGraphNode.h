// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Config/FGameFlowNodeInfo.h"
#include "Nodes/GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowGraphNode.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNodeAssetChanged)

/**
 * A node used inside Game Flow graphs.
 */
UCLASS(NotBlueprintable, NotBlueprintType)
class GAMEFLOWEDITOR_API UGameFlowGraphNode : public UEdGraphNode
{
	friend class UGameFlowNodeFactory;
	
	GENERATED_BODY()

public:
	/* Called when the node asset encapsulated inside this graph node gets changed. */
	FOnNodeAssetChanged OnNodeAssetChanged;
private:
	
	/* The game flow node asset encapsulated inside this graph node. */
	UPROPERTY()
	TObjectPtr<UGameFlowNode> NodeAsset;

	/* Node asset info red from global GameFlow plugin settings. */
	UPROPERTY()
	FGameFlowNodeInfo Info;
	
public:
	UGameFlowGraphNode();
	
	virtual void AllocateDefaultPins() override;
	virtual FName CreateUniquePinName(FName SourcePinName) const override;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool CanUserDeleteNode() const override;
	virtual void ReconstructNode() override;
	void OnAssetEdited();
	
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

	/**
	 * @brief Read GameFlow node asset and create graph node pins.
	 * @param PinDirection Whether you want to create a input or output pins.
	 * @param PinNames An array containing all the pin names.
	 */
	void CreateNodePins(const EEdGraphPinDirection PinDirection, const TArray<FName> PinNames);
	UEdGraphPin* CreateNodePin(const EEdGraphPinDirection PinDirection, FName PinName = EName::None);

protected:
	/* Initialize this node properties. */
	virtual void InitNode();
};

