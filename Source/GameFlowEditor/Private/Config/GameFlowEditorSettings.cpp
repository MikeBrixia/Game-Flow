// Fill out your copyright notice in the Description page of Project Settings.

#include "Config/GameFlowEditorSettings.h"

#include "GameFlowEditor.h"
#include "Config/GameFlowSettings.h"

UGameFlowEditorSettings::UGameFlowEditorSettings()
{
	bEditNodesStyles = false;
	ExecPinColor = FLinearColor::White;

	if (NodesTypes.Num() == 0)
	{
		FGameFlowNodeInfo ConditionalNodeInfo;
		ConditionalNodeInfo.TitleBarColor = FLinearColor::Gray;
		NodesTypes.Add(TEXT("Conditional"), ConditionalNodeInfo);

		FGameFlowNodeInfo LatentNodeInfo;
		LatentNodeInfo.TitleBarColor = FLinearColor(0.0, 0.24, 0.32, 1.0);
		NodesTypes.Add(TEXT("Latent"), LatentNodeInfo);

		FGameFlowNodeInfo EventNodeInfo;
		EventNodeInfo.TitleBarColor = FLinearColor(1.0, 0.48, 0.0, 1.0);
		NodesTypes.Add(TEXT("Event"), EventNodeInfo);

		FGameFlowNodeInfo DefaultNodeInfo;
		DefaultNodeInfo.TitleBarColor = FLinearColor(0.18, 0.34, 0.44, 1.0);
		NodesTypes.Add(TEXT("Default"), DefaultNodeInfo);

		FGameFlowNodeInfo InputNodeInfo;
		InputNodeInfo.TitleBarColor = FLinearColor(0.28, 0.0, 0.15, 1.0);
		NodesTypes.Add(TEXT("Input"), InputNodeInfo);

		FGameFlowNodeInfo OutputNodeInfo;
		InputNodeInfo.TitleBarColor = FLinearColor(0.28, 0.0, 0.15, 1.0);
		NodesTypes.Add(TEXT("Output"), OutputNodeInfo);

		FGameFlowNodeInfo DebugNodeInfo;
		DebugNodeInfo.TitleBarColor = FLinearColor(0.21, 0.0, 0.22, 1.0);
		NodesTypes.Add(TEXT("Debug"), DebugNodeInfo);
	}

	UE_LOG(LogGameFlow, Display, TEXT("Game Flow default settings initialized successfully!"));
}


void UGameFlowEditorSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
    
	// Reflect node types setting changed to UGameFlowNode option inside Runtime module.
	if(PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive && PropertyChangedEvent.GetPropertyName() == "NodesTypes")
	{
		TArray<FName> Options;
		NodesTypes.GenerateKeyArray(Options);
		
		UGameFlowSettings* RuntimeSettings = UGameFlowSettings::Get();
		// Share runtime useful properties with game flow editor settings.
		RuntimeSettings->Options = Options;
        RuntimeSettings->WireHighlightDuration = WireHighlightDuration;
	}
}
