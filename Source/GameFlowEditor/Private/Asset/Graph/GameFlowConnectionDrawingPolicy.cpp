#include "Asset/Graph/GameFlowConnectionDrawingPolicy.h"

#include "GameFlowEditor.h"
#include "Asset/Graph/GameFlowGraphSchema.h"

FConnectionDrawingPolicy* FGameFlowGraphConnectionDrawingPolicyFactory::CreateConnectionPolicy(
	const UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor,
	const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	FConnectionDrawingPolicy* ConnectionDrawingPolicy = nullptr;
	// Is the schema a GameFlowGraphSchema?
	if (Schema->IsA(UGameFlowGraphSchema::StaticClass()))
	{
		ConnectionDrawingPolicy = new FGameFlowConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements);
	}
	
	return ConnectionDrawingPolicy;
}

FGameFlowConnectionDrawingPolicy::FGameFlowConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID,
	                                                               float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements)
                                                                   : FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements)
{
	
}







