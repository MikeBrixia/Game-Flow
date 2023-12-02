#pragma once

#include "CoreMinimal.h"
#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "FGameFlowNodeInfo.generated.h"

/**
 * Stores info about a single type of node.
 */
USTRUCT()
struct FGameFlowNodeInfo
{
	GENERATED_BODY()

	FGameFlowNodeInfo();
	
	FGameFlowNodeInfo(const FSlateColor& TitleBarColor, const FSlateBrush& Icon, UClass* NodeClass);
	
	/* The color of the bar who contains the node name. */
	UPROPERTY(EditAnywhere)
	FSlateColor TitleBarColor;

	/* An icon that will appear in the top-left of the node.
	 * Should be use to visually identify a node category.
	 */
	UPROPERTY(EditAnywhere)
	FSlateBrush Icon;
	
	/* The source class of this specific graph node, controls which class should be
	 * used to create the graph node.
	 */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameFlowGraphNode> Class;
};
