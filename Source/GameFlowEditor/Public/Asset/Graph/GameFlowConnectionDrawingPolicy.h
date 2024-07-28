#pragma once

#include "ConnectionDrawingPolicy.h"
#include "EdGraphUtilities.h"
#include "GameFlowGraph.h"


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
	/** The graph in which the connections takes place. */
	TObjectPtr<UGameFlowGraph> GraphObj;

	/** How many time has passed since highlight start.*/
	double HighlightElapsedTime;

	double PreviousTime;
	
	/** How much the highlight should last. */
	float WireHighlightDuration;
public:
	
	FGameFlowConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor,
	                                 const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements);

	virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params) override;
	virtual void Draw(TMap<TSharedRef<SWidget>, FArrangedWidget>& InPinGeometries, FArrangedChildren& ArrangedNodes) override;
	void SetGraphObj(UGameFlowGraph* NewGraphObj);
    
protected:
	void HighlightConnection(FConnectionParams& Params);
	void NotHighlightedConnection(FConnectionParams& Params);
	void UpdateConnectionTimer(FPinConnectionInfo& ConnectionInfo);
};
