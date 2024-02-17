// Fill out your copyright notice in the Description page of Project Settings.

#include "Utils/UGameFlowNodeFactory.h"
#include "Asset/Graph/GameFlowGraphSchema.h"

UObject* UGameFlowNodeFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	return NewObject<UGameFlowNode>(InParent, InClass, InName, Flags, Context);
}
