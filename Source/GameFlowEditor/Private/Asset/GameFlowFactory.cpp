// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/GameFlowFactory.h"
#include "Flow.h"

UGameFlowFactory::UGameFlowFactory()
{
	SupportedClass = UFlow::StaticClass();
	bCreateNew = true;
	bEditAfterNew = false;
}

UObject* UGameFlowFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
										   UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UFlow>(InParent, InClass, InName, Flags, Context);
}


