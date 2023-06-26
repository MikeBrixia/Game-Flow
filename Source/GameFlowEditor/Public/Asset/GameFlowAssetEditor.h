#pragma once

class GameFlowAssetEditor
{
	
public:

	/* The root of the widget tree*/
	TSharedPtr<SWidget> WidgetTreeRoot;

	/* Call to initialize the Game Flow Asset editor*/
	virtual void InitEditor();
};
