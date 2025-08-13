#pragma once

#include "PinHandle.generated.h"

class UFloatProperty;
class UGameFlowNode;
class UPinHandle;

/** Utility structure used to handle game flow logical pin states and connections. */
UCLASS(DefaultToInstanced, Abstract)
class GAMEFLOW_API UPinHandle : public UObject
{
	GENERATED_BODY()

public:
	/** The name of the handled pin. */
	UPROPERTY(VisibleAnywhere)
	FName PinName;
	
	/** True if this is an output pin, false if it is in input pin. */
	UPROPERTY(VisibleAnywhere)
	bool bIsOutput;

private:
	UPROPERTY(TextExportTransient)
	TArray<UPinHandle*> Connections;

#if WITH_EDITORONLY_DATA

#endif

public:
	
	UPinHandle();

	virtual void TriggerPin();

	/** Get an array of pins connected to this pin. */
	virtual TArray<UPinHandle*> GetConnections();

	/** Get the node who owns this pin. */
	UGameFlowNode* GetNodeOwner() const;
	
protected:
	virtual void AddConnection(UPinHandle* Handle);
	virtual void RemoveConnection(UPinHandle* Handle);

#if WITH_EDITORONLY_DATA

public:
	/** True when this pin has been marked with a breakpoint, false otherwise. */
	UPROPERTY(TextExportTransient)
	bool bIsBreakpointEnabled;

	/** True if this pin is active and executing. */
	UPROPERTY(Transient, TextExportTransient)
	bool bIsActive;

	/** Time passed since this pin was activated. */
	UPROPERTY(Transient, TextExportTransient)
	double ActivatedElapsedTime;

	/** Last connections processing time. */
	UPROPERTY(Transient, TextExportTransient)
	double PreviousTime;
	
	/** Create a connection between this handle and another pin handle. */
	void CreateConnection(UPinHandle* OtherPinHandle);

	/** Cut a two-way connection between two nodes. */
	void CutConnection(UPinHandle* OtherPinHandle);

	/** Cut all two-way connections between this and other nodes. */
	void CutAllConnections();

	/**
	 * Check if this pin handle is valid and ready to be used.
	 * @return True if this handle is considered valid, false otherwise.
	 */
	bool IsValidHandle() const;

	/**
	 * Does this pin have a valid name?
	 * @return True if the pin name is considered valid, false otherwise.
	 */
	bool IsValidPinName() const;

	/**
	 * Check if you're allowed to create a connection between this and another node.
	 * @return True if the connection can be created, false otherwise.
	 */
	virtual bool CanCreateConnection(const UPinHandle* OtherPinHandle) const;

#endif
};
