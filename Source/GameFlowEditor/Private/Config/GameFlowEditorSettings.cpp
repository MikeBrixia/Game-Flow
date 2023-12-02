// Fill out your copyright notice in the Description page of Project Settings.

#include "Config/GameFlowEditorSettings.h"

UGameFlowEditorSettings::UGameFlowEditorSettings()
{
   // Initialize GameFlow default node types.
   const FGameFlowNodeInfo LatentNodeInfo (FColor::Emerald, FSlateBrush(), nullptr);
   NodesTypes.Add("Latent", LatentNodeInfo);
}
