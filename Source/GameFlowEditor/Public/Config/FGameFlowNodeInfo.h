#pragma once

#include "CoreMinimal.h"
#include "FGameFlowNodeInfo.generated.h"

/**
 * Stores info about a single type of node.
 */
USTRUCT()
struct FGameFlowNodeInfo
{
	GENERATED_BODY()

	FGameFlowNodeInfo();
	
	FGameFlowNodeInfo(const FSlateColor& TitleBarColor, const FSlateBrush& Icon);
	
	/* The color of the bar who contains the node name. */
	UPROPERTY(EditAnywhere)
	FSlateColor TitleBarColor;

	/* An icon that will appear in the top-left of the node.
	 * Should be use to visually identify a node category.
	 */
	UPROPERTY(EditAnywhere)
	FSlateBrush Icon;
};
