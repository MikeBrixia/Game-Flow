#pragma once

#include "Framework/Docking/TabManager.h"
#include "Templates/SharedPointer.h"

class GameFlowEditorUtils
{
	
public:
    
	/** Create Game Flow editor tabs
	 * @return Game Flow editor layout.
	 */
	static TSharedRef<FTabManager::FLayout> CreateEditorTabs();
};
