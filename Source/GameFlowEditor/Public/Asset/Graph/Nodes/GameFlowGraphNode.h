// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Config/FGameFlowNodeInfo.h"
#include "Config/GameFlowEditorSettings.h"
#include "Nodes/GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowGraphNode.generated.h"

/**
 * A node used inside Game Flow graphs.
 */
UCLASS(NotBlueprintable, NotBlueprintType)
class GAMEFLOWEDITOR_API UGameFlowGraphNode : public UEdGraphNode
{
	friend class UGameFlowNodeFactory;
	
	GENERATED_BODY()

private:
	
	/* The game flow node asset encapsulated inside this graph node. */
	UPROPERTY()
	TObjectPtr<UGameFlowNode> NodeAsset;

	/* Node asset info read from global GameFlow plugin settings. */
	UPROPERTY()
	FGameFlowNodeInfo Info;
	
public:
	UGameFlowGraphNode();
	
	virtual void AllocateDefaultPins() override;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	
	/* Get the asset contained inside this graph node. */
	FORCEINLINE UGameFlowNode* GetNodeAsset() const { return NodeAsset; }

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
	 * @param PinCategory Properties used to identify and create pins.
	 * @param PinDirection Whether you want to create a input or output pins.
	 * @param PinNames An array containing all the pin names.
	 */
	void CreateNodePins(const FEdGraphPinType PinCategory, const EEdGraphPinDirection PinDirection, const TArray<FName> PinNames);

	UEdGraphPin* CreateNodePin(const EEdGraphPinDirection PinDirection, FName PinName = EName::None);
protected:
	
	/* Initialize this node properties. */
	virtual void InitNode();
};
