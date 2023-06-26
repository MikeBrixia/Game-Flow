#pragma once

#include "AssetTypeActions_Base.h"

class FGameFlowAssetTypeAction :  public FAssetTypeActions_Base
{
public:

	FGameFlowAssetTypeAction();
	virtual uint32 GetCategories() override;
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override;
	void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;
};
