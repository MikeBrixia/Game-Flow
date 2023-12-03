// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Nodes/GameFlowNode.h"
#include "GameFlowEditorSettings.generated.h"

/**
 * Singleton configuration class for the Game Flow editor.
 */
UCLASS(Config=EditorPerProjectUserSettings, DefaultConfig, meta=(DisplayName="Game Flow"))
class GAMEFLOWEDITOR_API UGameFlowEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	FORCEINLINE static UGameFlowEditorSettings* Get()
	{
		UObject* DefaultObject = StaticClass()->GetDefaultObject();
		return CastChecked<UGameFlowEditorSettings>(DefaultObject);
	}
	
	UPROPERTY(Config, EditAnywhere, Category="Nodes")
	bool bEditNodesStyles;

	/* All the possible types of nodes available inside GameFlow editor.*/
	UPROPERTY(Config, EditAnywhere, Category="Nodes|Style", meta=(EditCondition="bEditNodesStyles"))
	TMap<FName, FGameFlowNodeInfo> NodesTypes;
	
	/* The color of the execution pin. */
	UPROPERTY(Config, EditAnywhere, Category="Nodes|Style", meta=(EditCondition="bEditNodesStyles"))
	FSlateColor ExecPinColor;

	/* This nodes will not be displayed inside the palette tab. */
	UPROPERTY(Config, EditAnywhere, Category="Nodes")
	TArray<TSubclassOf<UGameFlowNode>> HiddenFromPalette;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};
