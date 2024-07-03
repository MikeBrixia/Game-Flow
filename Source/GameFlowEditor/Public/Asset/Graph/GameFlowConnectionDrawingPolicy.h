#pragma once

#include "ConnectionDrawingPolicy.h"
#include "EdGraphUtilities.h"
#include "Nodes/GameFlowGraphNode.h"


/** Factory responsible for creating Game Flow graph connection policy.
 * this factory must be registered inside the module file,
 * using: FEdGraphUtilities::RegisterVisualPinConnectionFactory
 */
struct FGameFlowGraphConnectionDrawingPolicyFactory : public FGraphPanelPinConnectionFactory
{
public:
	
	virtual ~FGameFlowGraphConnectionDrawingPolicyFactory() override
	{
	}

	virtual FConnectionDrawingPolicy* CreateConnectionPolicy(const UEdGraphSchema* Schema, int32 InBackLayerID,
	                                                         int32 InFrontLayerID, float ZoomFactor,
	                                                         const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
	                                                         UEdGraph* InGraphObj) const override;
};

/**
 * Class responsible for handling connections between nodes and draw them.
 */
class FGameFlowConnectionDrawingPolicy : public FConnectionDrawingPolicy
{
public:
	FGameFlowConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor,
	                                 const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements);

	virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params) override;

	/** 
	 * Connections up to this node will not be highlighted anymore.
	 * @remarks Call chain can only be cleared on leaf and deactivated nodes.
	 * @returns Returns true if call chain can be cleared, false otherwise.
	 */
	bool CanClearCallChain(const UGameFlowGraphNode* Node) const;
};
