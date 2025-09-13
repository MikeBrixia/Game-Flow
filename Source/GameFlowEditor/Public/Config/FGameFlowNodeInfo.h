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
	
	FGameFlowNodeInfo(const FLinearColor& TitleBarColor, const FSlateBrush& Icon);
	
	/* The color of the bar that contains the node name. */
	UPROPERTY(EditAnywhere, Category="Default")
	FLinearColor TitleBarColor;

	/* An icon that will appear in the top-left of the node.
	 * Should be used to visually identify a node category.
	 */
	UPROPERTY(EditAnywhere, Category="Default")
	FSlateBrush Icon;
};
