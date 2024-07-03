
#include "Asset/Graph/GameFlowConnectionDrawingPolicy.h"

#include "GameFlowSubsystem.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "GameFlow/Public/Nodes/GameFlowNode.h"

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
	// Do not draw ending connection arrow.
	this->ArrowImage = nullptr;
	this->ArrowRadius = FVector2D::ZeroVector;
}

void FGameFlowConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin,
	FConnectionParams& Params)
{
	FConnectionDrawingPolicy::DetermineWiringStyle(OutputPin, InputPin, Params);
	
	// Execution flow highlight will only happen in PIE/SIE sessions.
	if(GEditor->IsPlayingSessionInEditor())
	{
		FWorldContext* WorldContext = GEditor->GetWorldContextFromPIEInstance(0);
		if(WorldContext != nullptr)
		{
			UWorld* PIE_PlayWorld = WorldContext->World();
			const UGameFlowGraphNode* FromNode = CastChecked<UGameFlowGraphNode>(OutputPin->GetOwningNode());
			const UGameFlowGraphNode* DestinationNode = CastChecked<UGameFlowGraphNode>(InputPin->GetOwningNode());
			UObject* AssetArchetype = CastChecked<UGameFlowGraph>(FromNode->GetGraph())->GameFlowAsset->GetArchetype();

			const UGameFlowSubsystem* Subsystem = PIE_PlayWorld->GetGameInstance()->GetSubsystem<UGameFlowSubsystem>();
			UGameFlowAsset* GameFlowAsset = Subsystem->GetRunningFlowByArchetype(AssetArchetype);

			// If we've found an instance of the asset archetype, then try debugging the link
			// between the two nodes.
			if(GameFlowAsset != nullptr)
			{
				const FName& FullyQualifiedInputPinName = FromNode->GetNodeAsset()->Outputs[OutputPin->PinName].GetFullPinName();
				const FName& FullyQualifiedOutputPinName = DestinationNode->GetNodeAsset()->Inputs[InputPin->PinName].GetFullPinName();

				// If destination is a leaf node(no output connections),
				// then we can clear this connection from the call chain.
				if(CanClearCallChain(DestinationNode))
				{
					GameFlowAsset->CallStack.Remove(FullyQualifiedInputPinName);
					GameFlowAsset->CallStack.Remove(FullyQualifiedOutputPinName);
				}

				// If this connection is registered inside the call stack, then highlight it.
				if(GameFlowAsset->CallStack.Contains(FullyQualifiedOutputPinName))
				{
					Params.WireColor = FColor::Orange;
					Params.WireThickness = 3.f;
					Params.bDrawBubbles = true;
				}
			}
		}
	}
}

bool FGameFlowConnectionDrawingPolicy::CanClearCallChain(const UGameFlowGraphNode* Node) const
{
	return Node->IsLeaf() && !Node->GetNodeAsset()->IsActiveNode();
}







