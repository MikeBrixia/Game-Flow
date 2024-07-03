// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowAsset.h"
#include "Nodes/GameFlowNode.h"
#include "UObject/Object.h"
#include "GameFlowNode_FlowControl_Subgraph.generated.h"

/**
 * Start execution of a game flow asset from another graph.
 * @remark Instanced subgraph will be a child of current graph.
 */
UCLASS(NotBlueprintType, Blueprintable, DisplayName="Subgraph", meta=(Category="Flow Control"))
class GAMEFLOW_API UGameFlowNode_FlowControl_Subgraph : public UGameFlowNode
{
	GENERATED_BODY()

public:

	/** Reference to the asset you want to instance. */
	UPROPERTY(EditAnywhere, Category="Game Flow|Subgraph")
	TSoftObjectPtr<UGameFlowAsset> Asset;

	UGameFlowNode_FlowControl_Subgraph();
	
	virtual void Execute_Implementation(const FName PinName) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	/** The instance of the referenced asset. */
	TObjectPtr<UGameFlowAsset> InstancedAsset;
	
	void ReconstructSubgraph();
    void ConstructInputPins();
    void ConstructOutputPins();
	
    bool CanInstanceAssetFromSource(const UGameFlowAsset* LoadedAsset) const;
    virtual void GetNodeIconInfo(FString& Key, FLinearColor& Color) const override;
#endif 
};
