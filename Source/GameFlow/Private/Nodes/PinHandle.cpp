
#include "Nodes/PinHandle.h"
#include "Nodes/GameFlowNode.h"

UPinHandle::UPinHandle()
{
#if WITH_EDITOR
	bIsBreakpointEnabled = false;
	bIsBreakpointPlaced = false;
#endif
}

void UPinHandle::TriggerPin()
{
#if WITH_EDITOR
	if (bIsBreakpointEnabled && OnPinTriggered.IsBound())
	{
		OnPinTriggered.Broadcast(this);
	}
#endif
}

TArray<UPinHandle*> UPinHandle::GetConnections()
{
	return Connections;
}

UGameFlowNode* UPinHandle::GetNodeOwner() const
{
	return GetTypedOuter<UGameFlowNode>();
}

bool UPinHandle::HasAnyConnections() const
{
	return Connections.Num() > 0;
}

bool UPinHandle::HasConnections(const UPinHandle* OtherPinHandle) const
{
	return Connections.Contains(OtherPinHandle);
}

#if WITH_EDITOR

void UPinHandle::CreateConnection(UPinHandle* OtherPinHandle)
{
	if(CanCreateConnection(OtherPinHandle))
	{
		// Create a two-way connection between the nodes.
		Connections.Add(OtherPinHandle);
		OtherPinHandle->Connections.Add(this);
	}
}

void UPinHandle::CutConnection(UPinHandle* OtherPinHandle)
{
	// Is the other pin connected to this pin?
	if (HasConnections(OtherPinHandle))
	{
		// If true, cut the connection between these two pins.
		Connections.Remove(OtherPinHandle);
		if (OtherPinHandle != nullptr)
		{
			OtherPinHandle->Connections.Remove(this);
		}
	}
}

void UPinHandle::CutAllConnections()
{
	for(const auto& Pin : GetConnections())
	{
        // After having found the connected pin, cut the connection.
		CutConnection(Pin);
	}
}

bool UPinHandle::IsValidHandle() const
{
	return IsValidPinName() && GetNodeOwner() != nullptr;
}

bool UPinHandle::IsValidPinName() const
{
	// A valid pin name should not be 'None' or whitespaces only.
	return !PinName.IsNone() && !PinName.ToString().IsEmpty();
}

bool UPinHandle::CanCreateConnection(const UPinHandle* OtherPinHandle) const
{
	const bool bValidHandles = IsValidHandle() && OtherPinHandle->IsValidHandle();
	// Do not allow a connection between two pins on the same node.
	const bool bRecursiveConnection = GetNodeOwner() == OtherPinHandle->GetNodeOwner();
	return bValidHandles && !bRecursiveConnection;
}

#endif

