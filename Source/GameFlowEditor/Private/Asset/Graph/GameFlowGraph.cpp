// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowGraph.h"
#include "GameFlowEditor.h"

class UGameFlowNode;

UGameFlowGraph::UGameFlowGraph()
{
}

void UGameFlowGraph::InitGraph()
{
	UE_LOG(LogGameFlow, Display, TEXT("Initializing game flow graph..."));
	
	const UEdGraphSchema* GraphSchema = GetSchema();
	
	// Create the default nodes which will appear in the graph
	// when the graph editor gets opened.
	GraphSchema->CreateDefaultNodesForGraph(*this);
}

bool UGameFlowGraph::CompileGraph(UObject* Asset)
{
	// True if compile was successful, false otherwise.
	bool bSuccess = false;

	return bSuccess;
}
