#include "Config/FGameFlowNodeInfo.h"

FGameFlowNodeInfo::FGameFlowNodeInfo()
{
}

FGameFlowNodeInfo::FGameFlowNodeInfo(const FSlateColor& TitleBarColor, const FSlateBrush& Icon, UClass* NodeClass)
{
	this->TitleBarColor = TitleBarColor;
	this->Icon = Icon;
	this->Class = NodeClass;
}
