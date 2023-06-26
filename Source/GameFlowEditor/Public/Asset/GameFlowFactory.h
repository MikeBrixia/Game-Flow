// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "GameFlowFactory.generated.h"

/**
 * Factory class to create GameFlow assets, graphs and nodes.
 */
UCLASS()
class GAMEFLOWEDITOR_API UGameFlowFactory : public UFactory
{
	GENERATED_BODY()

public:
	
	UGameFlowFactory();
	
	UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

