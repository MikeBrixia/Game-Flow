#include "Asset/GameFlowAssetEditor.h"

void GameFlowAssetEditor::InitEditor()
{
	// Create new layout.
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("GameFlowEditorLayout");

	// Create asset editor primary area.
	TSharedRef<FTabManager::FArea> PrimaryArea = FTabManager::NewPrimaryArea();

	// Create splitter and initialize it.
	TSharedRef<FTabManager::FSplitter> Splitter = FTabManager::NewSplitter();
	Splitter->SetSizeCoefficient(0.8f);
	Splitter->SetOrientation(Orient_Horizontal);

	// Create UI Stack and initialize it.
	TSharedRef<FTabManager::FStack> GraphTab = FTabManager::NewStack();
	GraphTab ->SetSizeCoefficient(0.8f);
	GraphTab ->AddTab("GameFlowGraph", ETabState::OpenedTab);

	// Create Details Stack and initialize it.
	TSharedRef<FTabManager::FStack> DetailsTab = FTabManager::NewStack();
	DetailsTab->SetSizeCoefficient(0.2f);
	DetailsTab->AddTab("GameFlowNodesPanel", ETabState::OpenedTab);

	// Split Graph and details tab.
	Splitter->Split(GraphTab);
	Splitter->Split(DetailsTab);
	
    PrimaryArea->Split(Splitter);

	Layout->AddArea(PrimaryArea);
}
