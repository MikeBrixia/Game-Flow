#pragma once

#include "GameFlowAsset.h"

class FGameFlowGraphDebugger
{
	
private:

	/** The debugged instance of the current edited GameFlowAsset. */
	TObjectPtr<UGameFlowAsset> DebuggedAssetInstance;
	
	/** Stores debug info needed by drawing policy. */
	TMap<FName, TPair<double, double>> PinsDebugInfo;

public:
	
	/**
	 * Set the instance of the inspected asset you want to debug.
	 * @remark Instance must be a child of the inspected game flow asset.
	 */
	void SetDebuggedInstance(UGameFlowAsset* Instance);
};
