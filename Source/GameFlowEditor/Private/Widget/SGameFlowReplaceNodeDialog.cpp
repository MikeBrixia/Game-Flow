// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/SGameFlowReplaceNodeDialog.h"
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "FGameFlowReplaceNodeWindow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGameFlowReplaceNodeDialog::Construct(const FArguments& InArgs)
{
	SCustomDialog::FArguments DialogArgs;
	DialogArgs.Title(LOCTEXT("ChooseReplacementClass", "Choose replacement class"));
	DialogArgs.AutoCloseOnButtonPress(InArgs._AutoCloseOnButtonPress);
	// Initialize custom dialog arguments.
	DialogArgs.Content()
	[
	    CreateDialogContent()
	];
	DialogArgs.Buttons({
			FButton(LOCTEXT("Replace", "Replace")),
			FButton(LOCTEXT("Cancel", "Cancel"))
	});
	
	// Construct custom dialog with supplied arguments.
	SCustomDialog::Construct(DialogArgs);
}

TSharedRef<SVerticalBox> SGameFlowReplaceNodeDialog::CreateDialogContent()
{
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");
	// Create class viewer widget with specified options.
	const TSharedRef<SWidget> ClassViewerWidget = ClassViewerModule.CreateClassViewer(GetOptions(),
		FOnClassPicked::CreateLambda([=](UClass* PickedClass)
	{
		this->PickedClass = PickedClass;
	}));
	
	TSharedRef<SVerticalBox> ReplaceNodeWidget = SNew(SVerticalBox)
	 + SVerticalBox::Slot()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		ClassViewerWidget
	]
	+ SVerticalBox::Slot()
	  .AutoHeight()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SCheckBox)
			.OnCheckStateChanged(this, &SGameFlowReplaceNodeDialog::OnCheckBoxStateChange)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(FMargin(0, 5))
		[
			SNew(STextBlock)
			.Text(INVTEXT("Replace all"))
		]
	];
	
	return ReplaceNodeWidget;
}

void SGameFlowReplaceNodeDialog::OnCheckBoxStateChange(ECheckBoxState State)
{
	bShouldReplaceAll = State == ECheckBoxState::Checked;
}

TSharedRef<IClassViewerFilter> SGameFlowReplaceNodeDialog::GetFilter() const
{
	return MakeShared<FGameFlowClassFilter>();
}

FClassViewerInitializationOptions SGameFlowReplaceNodeDialog::GetOptions() const
{
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.DisplayMode = EClassViewerDisplayMode::TreeView;
	Options.ClassFilters.Add(GetFilter());

	return Options;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE