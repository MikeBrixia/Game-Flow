// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/GameFlowGraph.h"
#include "GameFlowEditor.h"

UGameFlowGraph::UGameFlowGraph()
{
}

void UGameFlowGraph::InitGraph()
{
	UE_LOG(LogGameFlow, Display, TEXT("Initializing game flow graph..."));
	// Create the default nodes which will appear in the graph
	// when the graph editor gets opened.
	GetSchema()->CreateDefaultNodesForGraph(*this);
}
