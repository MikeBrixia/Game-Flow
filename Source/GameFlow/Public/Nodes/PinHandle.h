#pragma once

class UGameFlowNode;

/** Utility structure used to handle Game Flow I/O pins. */
USTRUCT(BlueprintType)
struct GAMEFLOW_API FPinHandle
{
	GENERATED_USTRUCT_BODY()
	
	FPinHandle();
	FPinHandle(const FPinHandle& Other);
	FPinHandle(TMap<FName, UGameFlowNode*> Connections, FName PinName);
	
	/** All the connections held by this pin. */
	UPROPERTY(VisibleAnywhere)
	TMap<FName, UGameFlowNode*> Connections;

	/** The name of the handled pin. */
	UPROPERTY(VisibleAnywhere)
	FName PinName;

	/** The node who owns this pin. */
    UPROPERTY()
    TObjectPtr<UGameFlowNode> PinOwner;

#if WITH_EDITOR
	/** Create a connection between this handle and another pin handle. */
    void CreateConnection(FPinHandle& OtherPinHandle);

	/** Check if this pin handle is valid and ready to be used. */
    bool IsValidHandle() const;
    bool CanCreateConnection(const FPinHandle& OtherPinHandle) const;
#endif 
};
