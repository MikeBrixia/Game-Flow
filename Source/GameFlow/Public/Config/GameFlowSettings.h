// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameFlowSettings.generated.h"

class UGameFlowNode;

/**
 * Singleton configuration class for Game Flow.
 */
UCLASS(Config=EditorPerProjectUserSettings, DefaultConfig, meta=(DisplayName="Game Flow"))
class GAMEFLOW_API UGameFlowSettings : public UDeveloperSettings
{
	
	GENERATED_BODY()

public:
	
	FORCEINLINE static UGameFlowSettings* Get()
	{
		UObject* DefaultObject = StaticClass()->GetDefaultObject();
		return CastChecked<UGameFlowSettings>(DefaultObject);
	}

	UPROPERTY()
	TArray<FName> Options;

	UPROPERTY()
	double WireHighlightDuration;
};