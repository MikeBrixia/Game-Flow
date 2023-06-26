// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Asset/GameFlowAssetTypeAction.h"
#include "Modules/ModuleManager.h"


DECLARE_LOG_CATEGORY_EXTERN(LogGameFlow, Display, All);

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
};