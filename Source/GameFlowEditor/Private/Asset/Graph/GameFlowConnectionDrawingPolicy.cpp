
#include "Asset/Graph/GameFlowConnectionDrawingPolicy.h"
#include "GameFlowSubsystem.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Config/GameFlowEditorSettings.h"
#include "Misc/App.h"

FConnectionDrawingPolicy* FGameFlowGraphConnectionDrawingPolicyFactory::CreateConnectionPolicy(
	const UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor,
	const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	UGameFlowGraph* GameFlowGraph = CastChecked<UGameFlowGraph>(InGraphObj);
	FGameFlowConnectionDrawingPolicy* GameFlowConnectionDrawingPolicy = new FGameFlowConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements);
	GameFlowConnectionDrawingPolicy->SetGraphObj(GameFlowGraph);
	
	return GameFlowConnectionDrawingPolicy;
}

FGameFlowConnectionDrawingPolicy::FGameFlowConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID,
                                                                   float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements)
                                                                   : FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements)
{
	this->EditorSettings = UGameFlowEditorSettings::Get();
	// Do not draw end connection arrow.
	this->ArrowImage = nullptr;
	this->ArrowRadius = FVector2D::ZeroVector;
}

void FGameFlowConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin,
	FConnectionParams& Params)
{
	// If no debugging instances could be found, fallback on default connection style.
	FConnectionDrawingPolicy::DetermineWiringStyle(OutputPin, InputPin, Params);
	
	// Apply custom game flow connection style only when there is a debugged asset instance.
	if (GraphObj->DebuggedAssetInstance != nullptr)
	{
		const UGameFlowGraphNode* FromNode = CastChecked<UGameFlowGraphNode>(OutputPin->GetOwningNode());
		const UGameFlowNode* FromNodeAsset = GraphObj->DebuggedAssetInstance->GetNodeByGUID(FromNode->NodeGuid);
		
		UPinHandle* FromPinHandle = FromNodeAsset->GetPinByName(OutputPin->PinName, EGPD_Output);

		const double WireHighlightDuration = EditorSettings->WireHighlightDuration;
		if (FromPinHandle->ActivatedElapsedTime <= 0)
		{
			// Start connection highlight timer as long as the two nodes are still active.
			FromPinHandle->ActivatedElapsedTime = WireHighlightDuration;
			FromPinHandle->bIsActive = false;
		}
		
		const double CurrentTime = FApp::GetCurrentTime();
		// Highlight timer, only functioning when connection is active.
		if(FromPinHandle->bIsActive)
		{
			const float Alpha = FromPinHandle->ActivatedElapsedTime / WireHighlightDuration;
			// Update connection params.
			Params.WireThickness = FMath::Lerp(.7f, 6.f, Alpha);
			Params.WireColor = FColor::Orange;
			Params.bDrawBubbles = true;
			
			// Keep decreasing the timer until it reaches 0.
			FromPinHandle->ActivatedElapsedTime -= CurrentTime - FromPinHandle->PreviousTime;
		}
		FromPinHandle->PreviousTime = CurrentTime;
	}
}

void FGameFlowConnectionDrawingPolicy::SetGraphObj(UGameFlowGraph* NewGraphObj)
{
	this->GraphObj = NewGraphObj;
}

void FGameFlowConnectionDrawingPolicy::HighlightConnection(FConnectionParams& Params)
{
	Params.WireColor = FColor::Orange;
	Params.WireThickness = 6.f;
	Params.bDrawBubbles = true;
}

void FGameFlowConnectionDrawingPolicy::NotHighlightedConnection(FConnectionParams& Params)
{
	Params.WireColor = FColor::White;
	Params.WireThickness = .7f;
	Params.bDrawBubbles = false;
}









