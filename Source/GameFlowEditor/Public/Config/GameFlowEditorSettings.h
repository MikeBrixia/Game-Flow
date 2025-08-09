// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FGameFlowNodeInfo.h"
#include "UObject/Object.h"
#include "GameFlowEditorSettings.generated.h"

/**
 * Singleton configuration class for the Game Flow editor.
 */
UCLASS(Config=EditorPerProjectUserSettings, DefaultConfig, meta=(DisplayName="Game Flow"))
class GAMEFLOWEDITOR_API UGameFlowEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UGameFlowEditorSettings();
	
	UPROPERTY(Config, EditAnywhere, Category="Nodes")
	bool bEditNodesStyles;

	/* All the possible types of nodes available inside GameFlow editor.*/
	UPROPERTY(Config, EditAnywhere, Category="Nodes|Style", meta=(EditCondition="bEditNodesStyles"))
	TMap<FName, FGameFlowNodeInfo> NodesTypes;
	
	/* The color of the execution pin. */
	UPROPERTY(Config, EditAnywhere, Category="Nodes|Style", meta=(EditCondition="bEditNodesStyles"))
	FLinearColor ExecPinColor;
    
	UPROPERTY(Config, EditAnywhere, Category="Nodes|Debug")
	double WireHighlightDuration = 2.f;
	
	FORCEINLINE static UGameFlowEditorSettings* Get()
	{
		UObject* DefaultObject = StaticClass()->GetDefaultObject();
		return CastChecked<UGameFlowEditorSettings>(DefaultObject);
	}
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};
