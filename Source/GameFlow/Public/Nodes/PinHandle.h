#pragma once

#include "PinHandle.generated.h"

class UFloatProperty;
class UGameFlowNode;
class UPinHandle;

UENUM()
enum EGameFlowPinType
{
	Exec,
	Property
};

/** Utility structure used to handle game flow logical pins state and connections. */
UCLASS(DefaultToInstanced)
class GAMEFLOW_API UPinHandle : public UObject
{
	GENERATED_BODY()
	
	UPinHandle();

public:
	/** The name of the handled pin. */
	UPROPERTY(VisibleAnywhere)
	FName PinName;

	/** The node who owns this pin. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGameFlowNode> PinOwner;

	/** True if this is an output pin, false if it is in input pin. */
    UPROPERTY(VisibleAnywhere)
    bool bIsOutput;

	/** All the connections held by this pin. */
	UPROPERTY(VisibleAnywhere)
	TMap<FName, UPinHandle*> Connections;

#if WITH_EDITORONLY_DATA
	/** True when this pin has been marked with a breakpoint, false otherwise. */
    UPROPERTY()
    bool bIsBreakpointEnabled;

	/** True if this pin is active and executing. */
	UPROPERTY(Transient)
	bool bIsActive;

	/** Time passed since this pin was activated. */
	UPROPERTY(Transient)
	double ActivatedElapsedTime;

	/** Last connections processing time. */
	UPROPERTY(Transient)
	double PreviousTime;
		
	/** The direction of the handled pin(input, output ecc.)*/
	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<EEdGraphPinDirection> PinDirection;
#endif
	
	 void TriggerPin();

#if WITH_EDITOR
	static UPinHandle* CreatePinHandle(FName PinName, UGameFlowNode* PinOwner, EEdGraphPinDirection PinDirection);
	/** Create a connection between this handle and another pin handle. */
    void CreateConnection(UPinHandle* OtherPinHandle);
	void CutConnection(UPinHandle* OtherPinHandle);
	void CutAllConnections();
	
	/** Check if this pin handle is valid and ready to be used. */
    bool IsValidHandle() const;
	bool IsValidPinName() const;
    bool CanCreateConnection(const UPinHandle* OtherPinHandle) const;
#endif
};



