// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameFlowGraphSchema.generated.h"

/**
 * The schema used by the GameFlow graph.
 * Schemas are used to manage and handle graph
 * startup node creation, Link breaks and much more.
 */
UCLASS()
class GAMEFLOWEDITOR_API UGameFlowGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	
	virtual FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID,
	                                                                float InZoomFactor,
	                                                                const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
	                                                                UEdGraph* InGraphObj) const override;
	
	
	FORCEINLINE virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Connect"));
	}
};
