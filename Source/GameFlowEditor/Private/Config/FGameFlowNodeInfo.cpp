#include "Config/FGameFlowNodeInfo.h"

FGameFlowNodeInfo::FGameFlowNodeInfo()
{
}

FGameFlowNodeInfo::FGameFlowNodeInfo(const FSlateColor& TitleBarColor, const FSlateBrush& Icon)
{
	this->TitleBarColor = TitleBarColor;
	this->Icon = Icon;
}
