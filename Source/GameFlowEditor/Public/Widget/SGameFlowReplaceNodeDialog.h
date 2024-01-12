// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "Dialog/SCustomDialog.h"
#include "Nodes/GameFlowNode.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Dialog used to help users picking a replacement class for a game flow node.
 * Usually used to replace dummy nodes with a valid and functional node.
 */
class SGameFlowReplaceNodeDialog : public SCustomDialog
{
public:
	
	SLATE_BEGIN_ARGS(SGameFlowReplaceNodeDialog)
		{
		   AutoCloseOnButtonPress(true);
		}
	/** Whether to automatically close this window when any button is pressed (default: true) */
	SLATE_ARGUMENT(bool, AutoCloseOnButtonPress)
	SLATE_END_ARGS()

private:
	/* The class currently picked by the user, false otherwise. */
	UClass* PickedClass = nullptr;
	/* True if the user has expressed the intent of replacing multiple nodes. */
	bool bShouldReplaceAll = false;
	
	void OnCheckBoxStateChange(ECheckBoxState State);
	
public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
    
	/* Get the currently picked class. */
	FORCEINLINE UClass* GetPickedClass() const { return PickedClass; }

	/* True if the user has expressed the intent of replacing multiple nodes, false otherwise. */
	FORCEINLINE bool ShouldReplaceAll() const { return bShouldReplaceAll; }
protected:
	virtual TSharedRef<SVerticalBox> CreateDialogContent();

	/* The filter applied to the class viewer. */
	virtual TSharedRef<IClassViewerFilter> GetFilter() const;

	/* Class viewer options. */
	virtual FClassViewerInitializationOptions GetOptions() const;
    
private:
    
	class FGameFlowClassFilter final : public IClassViewerFilter
	{
	public:
		/** All children of these classes will be included unless filtered out by another setting. */
		TSet<const UClass*> AllowedChildrenOfClasses;

		FGameFlowClassFilter()
		{
			AllowedChildrenOfClasses.Add(UGameFlowNode::StaticClass());
		}

		virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass,
									TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
		{
			return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
		}

		virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions,
											const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData,
											TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
		{
			return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) !=
				EFilterReturn::Failed;
		}
	};
};
