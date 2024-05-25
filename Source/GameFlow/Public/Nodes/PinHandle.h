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
	
	/** Check if this pin handle is valid and ready to be used. */
    bool IsValidHandle() const;
	bool IsValidPinName() const;
    bool CanCreateConnection(const FPinHandle& OtherPinHandle) const;
#endif
};


