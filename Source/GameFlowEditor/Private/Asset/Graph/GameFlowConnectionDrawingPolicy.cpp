
#include "Asset/Graph/GameFlowConnectionDrawingPolicy.h"
#include "GameFlowSubsystem.h"
#include "Asset/Graph/GameFlowGraphSchema.h"

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
	this->WireHighlightDuration = .7f;
	this->PreviousTime = 0.f;
	this->HighlightElapsedTime = 0.f;
	// Do not draw end connection arrow.
	this->ArrowImage = nullptr;
	this->ArrowRadius = FVector2D::ZeroVector;
}

void FGameFlowConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin,
	FConnectionParams& Params)
{
	// Apply custom game flow connection style only when there is a debugged asset instance.
	if (GraphObj->DebuggedAssetInstance != nullptr)
	{
		const UGameFlowGraphNode* FromNode = CastChecked<UGameFlowGraphNode>(OutputPin->GetOwningNode());
		const UGameFlowGraphNode* DestinationNode = CastChecked<UGameFlowGraphNode>(InputPin->GetOwningNode());

		UGameFlowNode* FromNodeAsset = GraphObj->DebuggedAssetInstance->GetNodeByGUID(
			FromNode->GetNodeAsset()->GUID);
		UGameFlowNode* DestinationNodeAsset = GraphObj->DebuggedAssetInstance->GetNodeByGUID(
			DestinationNode->GetNodeAsset()->GUID);

		FPinHandle FromPinHandle = FromNodeAsset->GetPinByName(OutputPin->PinName, EGPD_Output);
		FPinHandle DestinationPinHandle = DestinationNodeAsset->GetPinByName(InputPin->PinName, EGPD_Input);
		FPinConnectionInfo ConnectionInfo = FromPinHandle.Connections.FindRef(DestinationPinHandle.GetFullPinName());

		if (ConnectionInfo.HighlightElapsedTime < WireHighlightDuration)
		{
			// Highlight only active nodes connections.
			if (ConnectionInfo.bIsActive)
			{
				HighlightConnection(Params);
			}
		}
		else
		{
			// Start connection highlight timer as long as the two nodes are still active.
			ConnectionInfo.HighlightElapsedTime = 0.f;
			ConnectionInfo.PreviousTime = 0.f;
			ConnectionInfo.bIsActive = false;
			FConnectionDrawingPolicy::DetermineWiringStyle(OutputPin, InputPin, Params);
		}

		UpdateConnectionTimer(ConnectionInfo);
		// Update pin handle at the end of each connection style processing.
		FromPinHandle.UpdateConnection(ConnectionInfo);
	}
	else
	{
		// If no debugging instances could be found, fallback on default connection style.
		FConnectionDrawingPolicy::DetermineWiringStyle(OutputPin, InputPin, Params);
	}
	
}

void FGameFlowConnectionDrawingPolicy::SetGraphObj(UGameFlowGraph* NewGraphObj)
{
	this->GraphObj = NewGraphObj;
}

void FGameFlowConnectionDrawingPolicy::HighlightConnection(FConnectionParams& Params)
{
	Params.WireColor = FColor::Orange;
	Params.WireThickness = 3.f;
	Params.bDrawBubbles = true;
}

void FGameFlowConnectionDrawingPolicy::NotHighlightedConnection(FConnectionParams& Params)
{
	Params.WireColor = FColor::White;
	Params.WireThickness = .7f;
	Params.bDrawBubbles = false;
}

void FGameFlowConnectionDrawingPolicy::UpdateConnectionTimer(FPinConnectionInfo& ConnectionInfo)
{
	const double CurrentTime = FApp::GetCurrentTime();
	if (ConnectionInfo.PreviousTime == 0.f)
	{
		// Just set the previous time to current time for next update.
		ConnectionInfo.PreviousTime = CurrentTime;
	}
	else
	{
		// First we add the elapsed time between previous and current draw procedure.
		ConnectionInfo.HighlightElapsedTime += CurrentTime - ConnectionInfo.PreviousTime;
		// Then we update the previous time because we'll need it in the next draw procedure.
		ConnectionInfo.PreviousTime = CurrentTime;
	}
}









