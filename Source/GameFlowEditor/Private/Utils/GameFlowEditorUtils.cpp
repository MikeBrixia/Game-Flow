#include "Utils/GameFlowEditorUtils.h"

TSharedRef<FTabManager::FLayout> GameFlowEditorUtils::CreateEditorTabs()
{
	// Create new layout manager.
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("Game Flow Layout");

	// Create asset editor primary area.
	const TSharedRef<FTabManager::FArea> PrimaryArea = FTabManager::NewPrimaryArea();

	// Construct tabs layout.
	FTabManager::NewPrimaryArea()->SetOrientation(Orient_Horizontal)
	->Split
	(
	FTabManager::NewStack()
	      ->SetSizeCoefficient(0.2f)
	       ->AddTab("DetailsTab", ETabState::OpenedTab)
	)
	->Split
	(
	FTabManager::NewStack()
	      ->SetSizeCoefficient(0.6f)
	      ->AddTab("GraphTab", ETabState::OpenedTab)
	)
	->Split
	(
	FTabManager::NewStack()
          ->SetSizeCoefficient(0.2f)
          ->AddTab("NodesTab", ETabState::OpenedTab)
	);

	Layout->AddArea(PrimaryArea);
	
	return Layout;
}
