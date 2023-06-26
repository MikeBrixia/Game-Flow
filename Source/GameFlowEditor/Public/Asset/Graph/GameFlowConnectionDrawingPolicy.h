#pragma once
#include "ConnectionDrawingPolicy.h"
#include "EdGraphUtilities.h"

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
};
