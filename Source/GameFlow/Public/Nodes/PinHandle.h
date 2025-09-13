#pragma once

#include "PinHandle.generated.h"

class UFloatProperty;
class UGameFlowNode;
class UPinHandle;

#if WITH_EDITOR

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPinTriggered, UPinHandle*, PinHandle);

#endif

/**
 * UPinHandle class is responsible for managing and representing logical connections between nodes
 * in a GameFlow asset. It acts as an interface for interaction and data transfer between logical nodes.
 */
UCLASS(DefaultToInstanced, Abstract, EditInlineNew)
class GAMEFLOW_API UPinHandle : public UObject
{
	GENERATED_BODY()

public:
	/** The name of the handled pin. */
	UPROPERTY(VisibleAnywhere, Category="Default")
	FName PinName;

private:
	UPROPERTY(TextExportTransient)
	TArray<UPinHandle*> Connections;

public:
	
	UPinHandle();
	
	virtual void TriggerPin();

	/**
	 * Retrieves all pin handles that are currently connected to this pin handle.
	 *
	 * @return An array of pointers to the connected pin handles.
	 */
	virtual TArray<UPinHandle*> GetConnections();

	/**
	 * Get the node who owns this pin.
	 *
	 * @return The node who owns this pin.
	 */
	UGameFlowNode* GetNodeOwner() const;

	/**
	 * Checks if the current pin handle has any connections.
	 *
	 * @return True if the pin has one or more connections, false otherwise.
	 */
	virtual bool HasAnyConnections() const;

	/**
	 * Determines if the current pin handle is connected to the specified pin handle.
	 *
	 * @param OtherPinHandle A pointer to the other pin handle to check for a connection.
	 * @return True if the current pin handle is connected to the specified pin handle, false otherwise.
	 */
	virtual bool HasConnections(const UPinHandle* OtherPinHandle) const;

#if WITH_EDITORONLY_DATA

public:
	/**
	 * FOnPinTriggered is a delegate used to notify when a pin is triggered during execution in the GameFlow system.
	 * It facilitates communication or events handling tied to pin activation or processing.
	 */
	FOnPinTriggered OnPinTriggered;
	
	UPROPERTY(TextExportTransient)
    bool bIsBreakpointPlaced;
	
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
