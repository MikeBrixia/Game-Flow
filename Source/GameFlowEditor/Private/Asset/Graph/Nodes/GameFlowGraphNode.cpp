// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/Graph/Nodes/GameFlowGraphNode.h"
#include "GameFlowAsset.h"
#include "Asset/Graph/GameFlowGraphSchema.h"
#include "Config/FGameFlowNodeInfo.h"
#include "Config/GameFlowEditorSettings.h"
#include "Misc/ITransactionObjectAnnotation.h"
#include "Misc/TransactionObjectEvent.h"
#include "Utils/UGameFlowNodeFactory.h"
#include "Widget/Nodes/SGameFlowNode.h"

UGameFlowGraphNode::UGameFlowGraphNode()
{
}

void UGameFlowGraphNode::InitNode()
{
	// Vital assertions.
	checkf(NodeAsset != nullptr, TEXT("Node asset is invalid(nullptr)"));

	// Initialize node.
	CreateNodePins(false);
	
	UGameFlowEditorSettings* Settings = UGameFlowEditorSettings::Get();
	// Get node asset info from config.
	Info = Settings->NodesTypes.FindChecked(NodeAsset->TypeName);

	NodeAsset->OnAssetRedirected.AddUObject(this, &UGameFlowGraphNode::OnLiveOrHotReloadCompile);
	// This is the only way to listen to blueprint compile events(at least the one i've found).
	GEditor->OnBlueprintCompiled().AddUObject(this, &UGameFlowGraphNode::OnAssetCompiled);
	GEditor->OnBlueprintPreCompile().AddUObject(this, &UGameFlowGraphNode::OnAssetBlueprintPreCompiled);
}

void UGameFlowGraphNode::OnAssetSelected(const FAssetData& AssetData)
{
}

void UGameFlowGraphNode::OnAssetValidated()
{
	const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	GraphSchema->ValidateNodeAsset(this);
	// Notify listeners this node has been validated.
	OnValidationResult.Broadcast();
}

void UGameFlowGraphNode::OnLiveOrHotReloadCompile()
{
	// Mark as pending compilation on cpp compile(live coding or hot reload).
	bPendingCompilation = true;
	// Call default asset compilation callback.
	OnAssetCompiled();
}

void UGameFlowGraphNode::OnAssetCompiled()
{
	// Reconstruct node only if it is pending compile.
	if(bPendingCompilation)
	{
		//UE_LOG(LogGameFlow, Display, TEXT("Cpp compile event received for %s"), *NodeAsset->GetName())
		const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
		// Ensure compiled asset is valid.
		GraphSchema->ValidateNodeAsset(this);
		// reconstruct the compiled asset with the updated properties/logic.
		ReconstructNode();
		
		// Notify listeners this node has been compiled.
		OnNodeAssetChanged.Broadcast();

		// Node has already been compiled, remove the mark from it.
		bPendingCompilation = false;
	}
}

void UGameFlowGraphNode::OnAssetBlueprintPreCompiled(UBlueprint* Blueprint)
{
	// If the compiled node is the graph encapsulated node, mark it as a pending compile graph node.
	bPendingCompilation = Blueprint != nullptr && Blueprint == NodeAsset->GetClass()->ClassGeneratedBy;
}

void UGameFlowGraphNode::OnDummyReplacement(UClass* ClassToReplace)
{
	const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	
	UGameFlowAsset* GameFlowAsset = NodeAsset->GetTypedOuter<UGameFlowAsset>();
	UGameFlowNode* SubstituteNodeAsset = UGameFlowNodeFactory::CreateGameFlowNode(ClassToReplace, GameFlowAsset);
	
	FObjectInstancingGraph ObjectInstancingGraph;
	ObjectInstancingGraph.AddNewObject(SubstituteNodeAsset, NodeAsset);
	TSet<FName> InOutExtraNames;
	UGameFlowGraphNode* SubstituteNode = CastChecked<UGameFlowGraphNode>(
		GraphSchema->CreateSubstituteNode(this, GetGraph(), &ObjectInstancingGraph, InOutExtraNames)
		);
	
	DestroyNode();
	// Recompile substitute node; this action will update the actual game flow asset.
	GraphSchema->CompileGraphNode(SubstituteNode, TArray { EGPD_Input, EGPD_Output });
}

void UGameFlowGraphNode::AllocateDefaultPins()
{
	CreateNodePins(false);
}

FName UGameFlowGraphNode::CreateUniquePinName(FName SourcePinName) const
{
	FString GeneratedName = SourcePinName.ToString();
	int Number;
	if(GeneratedName.IsNumeric())
	{
		Number = FCString::Atoi(*GeneratedName);
		GeneratedName = FString::FromInt(Number + 1);
	}
	else
	{
		Number = GetNum(GeneratedName);
		GeneratedName = FString::Printf(TEXT("NewPin_%d"), Number + 1);
	}

	return FName(GeneratedName);
}

TSharedPtr<SGraphNode> UGameFlowGraphNode::CreateVisualWidget()
{
	// Use UCLASS display name attribute value as node title.
	const FText TitleText = GetNodeTitle(ENodeTitleType::EditableTitle);
	
	TSharedRef<SGameFlowNode> NodeWidget = SNew(SGameFlowNode)
		                                   .Node(this)
		                                   .TitleText(TitleText);
	
    // Validate node asset.
	CastChecked<UGameFlowGraphSchema>(GetSchema())->ValidateNodeAsset(this);
	
	// Create and initialize node widget.
	return NodeWidget;
}

FText UGameFlowGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NodeAsset->GetClass()->GetDisplayNameText();
}

bool UGameFlowGraphNode::IsOrphan() const
{
	// Find all input pins which have a connection.
	const TArray<UEdGraphPin*> ConnectedInputPins = Pins.FilterByPredicate([] (const UEdGraphPin* Pin)
	{
		return Pin->Direction == EGPD_Input && Pin->HasAnyConnections();
	});

	// If none of the input pins have at least
	// one connection, this node is orphan.
	return ConnectedInputPins.Num() == 0;
}

bool UGameFlowGraphNode::CanUserDeleteNode() const
{
	const FText NodeDisplayName = NodeAsset->GetClass()->GetDisplayNameText();
	// User will be able to delete all types of nodes except 'Start' and 'Finish'
	return !(NodeDisplayName.EqualTo(INVTEXT("Start")) || NodeDisplayName.EqualTo(INVTEXT("Finish")));
}

void UGameFlowGraphNode::ReconstructNode()
{
	const UGameFlowGraphSchema* GraphSchema = CastChecked<UGameFlowGraphSchema>(GetSchema());
	
	const UGameFlowEditorSettings* GameFlowEditorSettings = UGameFlowEditorSettings::Get();
	Info = GameFlowEditorSettings->NodesTypes.FindRef(NodeAsset->TypeName);
	
	// Reallocate all node pins.
	Pins.Empty();
	CreateNodePins(false);
	
	const UGameFlowGraph& GameFlowGraph = *CastChecked<UGameFlowGraph>(GetGraph());
	
	// Recompile node and recreate it's node connections.
	GraphSchema->RecreateNodeConnections(GameFlowGraph, this, TArray { EGPD_Input, EGPD_Output });
	GraphSchema->CompileGraphNode(this, TArray { EGPD_Input, EGPD_Output});
}

void UGameFlowGraphNode::ReportError(EMessageSeverity::Type MessageSeverity)
{
	bHasCompilerMessage = MessageSeverity == EMessageSeverity::Error ||
		    MessageSeverity == EMessageSeverity::Warning ||
		    MessageSeverity == EMessageSeverity::Info ||
			MessageSeverity == EMessageSeverity::PerformanceWarning;
	ErrorType = MessageSeverity;
}

void UGameFlowGraphNode::SetNodeAsset(UGameFlowNode* Node)
{
	NodeAsset = Node;
	// Read new info data from config using the new node asset type.
	UGameFlowEditorSettings* Settings = UGameFlowEditorSettings::Get();
	Info = Settings->NodesTypes.FindChecked(NodeAsset->TypeName);
	
	// Notify listeners that the node asset has been changed.
	if(OnNodeAssetChanged.IsBound())
	{
		OnNodeAssetChanged.Broadcast();
	}
}

void UGameFlowGraphNode::CreateNodePins(bool bAddToAsset)
{
	// Read input pins names from node asset and create graph pins.
	for(const FName& PinName : NodeAsset->GetInputPins())
	{
		CreateNodePin(EGPD_Input, PinName, bAddToAsset);
	}

	// Read output pins names from node asset and create graph pins.
	for(const FName& PinName : NodeAsset->GetOutputPins())
	{
		CreateNodePin(EGPD_Output, PinName, bAddToAsset);
	}
}

UEdGraphPin* UGameFlowGraphNode::CreateNodePin(const EEdGraphPinDirection PinDirection, FName PinName, bool bAddToAsset)
{
	if(bAddToAsset)
	{
		switch(PinDirection)
		{
			// Direction is not valid, do nothing.
			default: break;
			
			// Add input pin to node asset.
			case EGPD_Input:
				{
					TArray<FName> InputPins = NodeAsset->GetInputPins();
					const FName PreviousName = InputPins.Num() > 0 ? InputPins.Last() : "None";
					PinName = CreateUniquePinName(PreviousName);
					NodeAsset->AddInput(PinName, {});
					
					break;
				}
			// Add output pin to node asset.	
			case EGPD_Output:
				{
					TArray<FName> OutputPins = NodeAsset->GetOutputPins();
					const FName PreviousName = OutputPins.Num() > 0 ? OutputPins.Last() : "None";
					PinName = CreateUniquePinName(PreviousName);
					NodeAsset->AddOutput(PinName, {});
					
					break;
				}
		}
	}
	
	UEdGraphPin* Pin = nullptr;
	// Create pin object only if name is valid.
	if(!PinName.IsEqual(EName::None))
	{
		const FEdGraphPinType PinType = GetGraphPinType();
		// Create the pin object.
		Pin = CreatePin(PinDirection, PinType, PinName);
		Pin->PinFriendlyName = FText::FromName(PinName);
	}
	return Pin;
}

