#pragma once

#include "ConnectionDrawingPolicy.h"
#include "EdGraphUtilities.h"
#include "GameFlowGraph.h"
#include "Config/GameFlowEditorSettings.h"


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

	TObjectPtr<UGameFlowEditorSettings> EditorSettings;
public:
	
	FGameFlowConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor,
	                                 const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements);

	virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params) override;
	void SetGraphObj(UGameFlowGraph* NewGraphObj);
    
protected:
	void HighlightConnection(FConnectionParams& Params);
	void NotHighlightedConnection(FConnectionParams& Params);
};
