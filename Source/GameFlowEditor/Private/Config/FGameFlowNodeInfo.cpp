#include "Config/FGameFlowNodeInfo.h"

FGameFlowNodeInfo::FGameFlowNodeInfo()
{
}

FGameFlowNodeInfo::FGameFlowNodeInfo(const FLinearColor& TitleBarColor, const FSlateBrush& Icon)
{
	this->TitleBarColor = TitleBarColor;
	this->Icon = Icon;
}
