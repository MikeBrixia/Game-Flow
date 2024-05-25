// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/Flow/GameFlowNode_FlowControl_Subgraph.h"
#include "GameFlowSubsystem.h"
#include "Engine/StreamableManager.h"

UGameFlowNode_FlowControl_Subgraph::UGameFlowNode_FlowControl_Subgraph()
{
	TypeName = "Conditional";
}

void UGameFlowNode_FlowControl_Subgraph::Execute_Implementation(const FName& PinName)
{
	Super::Execute_Implementation(PinName);
	InstancedAsset->Execute(PinName);
}

#if WITH_EDITOR

void UGameFlowNode_FlowControl_Subgraph::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(PropertyChangedEvent.GetPropertyName().IsEqual("Asset"))
	{
		FStreamableManager StreamableManager;
		// Load asset from soft reference.
		const UGameFlowAsset* LoadedAsset = StreamableManager.LoadSynchronous(Asset);
		
		// Loaded asset should be of a different class.
		if(CanInstanceAssetFromSource(LoadedAsset))
		{
			// Create an instance of the loaded asset and register it inside game flow.
			InstancedAsset = LoadedAsset->CreateInstance(this);
		
			// Once we've done initializing the instance, it is time to
			// rebuild modified subgraph to match instanced asset I/O pins.
			ReconstructSubgraph();

			// Call this to update graph node look.
			OnAssetRedirected.Broadcast();
		}
		else
		{
			OnErrorEvent.Broadcast(EMessageSeverity::Error, "Subgraph recursion not allowed");
		}
	}
}

void UGameFlowNode_FlowControl_Subgraph::ReconstructSubgraph()
{
	Inputs.Empty();
	Outputs.Empty();
	ConstructInputPins();
	ConstructOutputPins();
}

void UGameFlowNode_FlowControl_Subgraph::ConstructInputPins()
{
	if(InstancedAsset != nullptr)
	{
		TArray<FName> SubgraphInputPins;
		InstancedAsset->CustomInputs.GenerateKeyArray(SubgraphInputPins);
		for(const FName& PinName : SubgraphInputPins)
		{
			AddInputPin(PinName);
		}
	}
}

void UGameFlowNode_FlowControl_Subgraph::ConstructOutputPins()
{
	if(InstancedAsset != nullptr)
	{
		TArray<FName> SubgraphOutputPins;
		InstancedAsset->CustomOutputs.GenerateKeyArray(SubgraphOutputPins);
		for(const FName& PinName : SubgraphOutputPins)
		{
			AddOutputPin(PinName);
		}
	}
}

bool UGameFlowNode_FlowControl_Subgraph::CanInstanceAssetFromSource(const UGameFlowAsset* LoadedAsset) const
{
	return LoadedAsset != nullptr && LoadedAsset != GetTypedOuter<UGameFlowAsset>();
}

#endif
