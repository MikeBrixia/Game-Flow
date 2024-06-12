// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowEditor.h"
#include "Config/FGameFlowNodeInfo.h"
#include "Nodes/GameFlowNode.h"
#include "Nodes/GameFlowNode_Input.h"
#include "Nodes/GameFlowNode_Output.h"
#include "UObject/Object.h"
#include "GameFlowGraphNode.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNodeAssetChanged)
DECLARE_MULTICAST_DELEGATE(FOnValidationEnd)

/**
 * A node used inside Game Flow graphs.
 */
UCLASS(NotBlueprintable, NotBlueprintType)
class GAMEFLOWEDITOR_API UGameFlowGraphNode : public UEdGraphNode
{
	friend class UGameFlowNodeFactory;
	friend struct FGameFlowNodeSchemaAction_CreateOrDestroyNode;
	
	GENERATED_BODY()

public:
	/** Called when the node asset encapsulated inside this graph node gets changed. */
	FOnNodeAssetChanged OnNodeAssetChanged;
	/** Callback for when this node has finished being validated. */
	FOnValidationEnd OnValidationResult;

	/** True if this node is currently inside a rebuild process. */
	bool bIsRebuilding = false;
private:
	
	/** The game flow node asset encapsulated inside this graph node. */
	UPROPERTY()
	TObjectPtr<UGameFlowNode> NodeAsset;

	/** Node asset info red from global GameFlow plugin settings. */
	UPROPERTY()
	FGameFlowNodeInfo Info;

	/** List of game flow graph node context menu command actions. */
	TSharedPtr<FUICommandList> ContextMenuCommands;
	
	/** True if the node asset is waiting to be compiled. */
	bool bPendingCompilation;
	
public:
	UGameFlowGraphNode();
	
	UEdGraphPin* CreateNodePin(const EEdGraphPinDirection PinDirection, FName PinName = EName::None, bool bAddToAsset = true);
	virtual FName CreateUniquePinName(FName SourcePinName) const override;
	virtual void AllocateDefaultPins() override;
    virtual void OnPinRemoved(UEdGraphPin* InRemovedPin) override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	FEdGraphPinType GetGraphPinType() const;
	
	void OnLiveOrHotReloadCompile();
	void OnAssetCompiled();
	void OnAssetBlueprintPreCompiled(UBlueprint* Blueprint);
	void OnAssetValidated();
	void OnAssetSelected(const FAssetData& AssetData);

	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	void ReportError(EMessageSeverity::Type MessageSeverity, FString ErrorMessage);
	
	UGameFlowNode* GetNodeAsset() const;
    void SetNodeAsset(UGameFlowNode* Node);
	FGameFlowNodeInfo& GetNodeInfo();
	void SetNodeInfo(FGameFlowNodeInfo NewInfo);
	virtual FText GetTooltipText() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	
	virtual void PostPlacedNewNode() override;
	virtual void DestroyNode() override;
	virtual void OnRenameNode(const FString& NewName) override;
	virtual void ReconstructNode() override;
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	
	bool IsRoot() const;
	bool IsOrphan() const;
	virtual bool CanUserDeleteNode() const override;
	virtual bool CanBeReplaced() const;
	virtual bool GetCanRenameNode() const override;
	virtual bool ShowPaletteIconOnNode() const override;

private:
	
	void OnReplacementRequest();
	void OnValidationRequest();
	void OnAddBreakpointRequest();
	void OnRemoveBreakpointRequest();
	void OnDisableBreakpointRequest();
	void OnEnableBreakpointRequest();
	
	bool CanAddBreakpoint() const;
	bool CanRemoveBreakpoint() const;
	bool CanEnableBreakpoint() const;
	bool CanDisableBreakpoint() const;
	
	void ConfigureContextMenuAction();
};

