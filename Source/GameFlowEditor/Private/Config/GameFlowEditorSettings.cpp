// Fill out your copyright notice in the Description page of Project Settings.

#include "Config/GameFlowEditorSettings.h"
#include "Config/GameFlowSettings.h"

void UGameFlowEditorSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
    
	// Reflect node types setting changed to UGameFlowNode option inside Runtime module.
	if(PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive && PropertyChangedEvent.GetPropertyName() == "NodesTypes")
	{
		TArray<FName> Options;
		NodesTypes.GenerateKeyArray(Options);
		UGameFlowSettings::Get()->Options = Options;
	}
}
