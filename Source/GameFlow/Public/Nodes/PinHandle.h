#pragma once

#include "PinHandle.generated.h"

class UGameFlowNode;

/**
 * Serializable alternative to TPair for storing
 * Input pins name and nodes in Game Flow.
 */
USTRUCT(BlueprintType)
struct GAMEFLOW_API FGameFlowPinNodePair
{
	GENERATED_USTRUCT_BODY()

	FGameFlowPinNodePair();
	FGameFlowPinNodePair(const FName& InputPinName, UGameFlowNode* Node);

	UPROPERTY(EditAnywhere)
	FName OtherPinName;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UGameFlowNode> Node;

	friend bool operator==(const FGameFlowPinNodePair& Lhs, const FGameFlowPinNodePair& RHS)
	{
		return Lhs.OtherPinName == RHS.OtherPinName
			&& Lhs.Node == RHS.Node;
	}

	friend bool operator!=(const FGameFlowPinNodePair& Lhs, const FGameFlowPinNodePair& RHS)
	{
		return !(Lhs == RHS);
	}
};


/** Utility structure used to handle game flow logical pins state and connections. */
USTRUCT(BlueprintType)
struct GAMEFLOW_API FPinHandle
{
	GENERATED_USTRUCT_BODY()
	
	FPinHandle();
	FPinHandle(FName PinName);
	FPinHandle(const FPinHandle& Other);
	FPinHandle(TArray<FGameFlowPinNodePair>Connections, FName PinName);
	
	/** All the connections held by this pin. */
	UPROPERTY(VisibleAnywhere)
	TArray<FGameFlowPinNodePair> Connections;

	/** The name of the handled pin. */
	UPROPERTY(VisibleAnywhere)
	FName PinName;

	/** The node who owns this pin. */
    UPROPERTY()
    TObjectPtr<UGameFlowNode> PinOwner;

#if WITH_EDITORONLY_DATA
	/** True when this pin has been marked with a breakpoint, false otherwise. */
    UPROPERTY()
    bool bIsBreakpointEnabled;

	/** The direction of the handled pin(input, output ecc.)*/
    UPROPERTY(VisibleDefaultsOnly)
    TEnumAsByte<EEdGraphPinDirection> PinDirection;
	
	/** Create a connection between this handle and another pin handle. */
    void CreateConnection(FPinHandle& OtherPinHandle);
	void CutConnection(FPinHandle& OtherPinHandle);
	void CutAllConnections();
    void RenamePin(FName NewPinName);
	
	/** Check if this pin handle is valid and ready to be used. */
    bool IsValidHandle() const;
	bool IsValidPinName() const;
    bool CanCreateConnection(const FPinHandle& OtherPinHandle) const;
#endif 
};
