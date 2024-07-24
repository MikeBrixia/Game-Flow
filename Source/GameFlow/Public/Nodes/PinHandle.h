#pragma once

#include "PinHandle.generated.h"

class UFloatProperty;
class UGameFlowNode;
struct FPinHandle;

UENUM()
enum EGameFlowPinType
{
	Exec,
	Property
};

/**
 * Stores all relevant info about a connection between two pins.
 */
USTRUCT(BlueprintType)
struct GAMEFLOW_API FPinConnectionInfo
{
	GENERATED_USTRUCT_BODY()

	FPinConnectionInfo();
	FPinConnectionInfo(const FName& InputPinName, UGameFlowNode* Node);
	
	UPROPERTY(EditAnywhere)
	FName DestinationPinName;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UGameFlowNode> DestinationObject;

// Data needed by Game Flow drawing policy to correctly display execution flow of nodes.
#if WITH_EDITORONLY_DATA
	/** How many time has passed since highlight start. */
	UPROPERTY(Transient)
	double HighlightElapsedTime;
     
	/** Last connections processing time. */
	UPROPERTY(Transient)
	double PreviousTime;

	/** True if the connection is currently being highlighted. */
    UPROPERTY()
    bool bIsActive;
#endif
	
	friend bool operator==(const FPinConnectionInfo& Lhs, const FPinConnectionInfo& RHS)
	{
		return Lhs.DestinationPinName == RHS.DestinationPinName
			&& Lhs.DestinationObject == RHS.DestinationObject;
	}

	friend bool operator!=(const FPinConnectionInfo& Lhs, const FPinConnectionInfo& RHS)
	{
		return !(Lhs == RHS);
	}
};

/** Utility structure used to handle game flow logical pins state and connections. */
USTRUCT()
struct GAMEFLOW_API FPinHandle
{
	GENERATED_USTRUCT_BODY()
	
	FPinHandle();
	FPinHandle(FName PinName, UGameFlowNode* PinOwner, TEnumAsByte<EEdGraphPinDirection> PinDirection);
	FPinHandle(const FPinHandle& Other);
	
	/** The name of the handled pin. */
	UPROPERTY(VisibleAnywhere)
	FName PinName;

	/** All the connections held by this pin. */
    UPROPERTY(VisibleAnywhere)
    TMap<FName, FPinConnectionInfo> Connections;

#if WITH_EDITORONLY_DATA
	/** The node who owns this pin. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGameFlowNode> PinOwner;
	
	/** True when this pin has been marked with a breakpoint, false otherwise. */
    UPROPERTY()
    bool bIsBreakpointEnabled;

	/** The direction of the handled pin(input, output ecc.)*/
    UPROPERTY(VisibleDefaultsOnly)
    TEnumAsByte<EEdGraphPinDirection> PinDirection;
#endif

#if WITH_EDITOR
	/** Create a connection between this handle and another pin handle. */
    void CreateConnection(FPinHandle& OtherPinHandle);
	void CutConnection(FPinHandle& OtherPinHandle);
	void CutAllConnections();
	void UpdateConnection(FPinConnectionInfo& ConnectionInfo);
	
	/** Check if this pin handle is valid and ready to be used. */
    bool IsValidHandle() const;
	bool IsValidPinName() const;
    bool CanCreateConnection(const FPinHandle& OtherPinHandle) const;
    FName GetFullPinName() const;
#endif
};


