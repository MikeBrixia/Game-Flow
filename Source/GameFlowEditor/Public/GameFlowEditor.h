// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Asset/GameFlowAssetTypeAction.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameFlow, Display, All);

class UGameFlowNode;

class FGameFlowEditorModule final : public IModuleInterface
{
public:
	
	/* Action needed to create Game Flow assets. */
	TSharedPtr<FGameFlowAssetTypeAction> GameFlowAsset;

	/* The category of the game flow asset. */
	static EAssetTypeCategories::Type GameFlowCategory;
	
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OnPostEngineInit();
	
private:
	void OnBlueprintCompiled();
	void OnBlueprintPreCompile(UBlueprint* Blueprint);
	
#if WITH_HOT_RELOAD || WITH_LIVE_CODING
	void OnHotReload(EReloadCompleteReason ReloadCompleteReason);
#endif
    /** Used to realign node pins with the CDO after a compilation event. */
	void PostCompilePinsFixup(TArray<FDiffSingleResult> Diff, UGameFlowNode* Node, EEdGraphPinDirection PinDirection);
	void InitializeCppScriptTemplates();
	void ForwardEditorSettingsToRuntimeSettings();
	void RemoveCppScriptTemplates();
	
	FORCEINLINE FString GetScriptTemplatesPath() const { return FPaths::ProjectPluginsDir() / "GameFlow/Config/ScriptTemplates/"; }
	FORCEINLINE FString GetEngineScriptTemplatesPath() const { return FPaths::EngineContentDir() / "Editor/Templates/"; }
};