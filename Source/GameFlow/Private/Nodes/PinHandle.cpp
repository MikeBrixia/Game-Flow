
#include "Nodes/PinHandle.h"
#include "Config/GameFlowSettings.h"
#include "Nodes/GameFlowNode.h"

UPinHandle::UPinHandle()
{
#if WITH_EDITOR
	bIsBreakpointEnabled = false;
#endif
}

void UPinHandle::TriggerPin()
{
}

TArray<UPinHandle*> UPinHandle::GetConnections()
{
	return Connections;
}

UGameFlowNode* UPinHandle::GetNodeOwner() const
{
	return GetTypedOuter<UGameFlowNode>();
}

void UPinHandle::AddConnection(UPinHandle* Handle)
{
	Connections.Add(Handle);
}

void UPinHandle::RemoveConnection(UPinHandle* Handle)
{
	Connections.Remove(Handle);
}

#if WITH_EDITOR

void UPinHandle::CreateConnection(UPinHandle* OtherPinHandle)
{
	if(CanCreateConnection(OtherPinHandle))
	{
		// Create a two way connection between the nodes.
		AddConnection(OtherPinHandle);
		OtherPinHandle->AddConnection(this);
	}
}

void UPinHandle::CutConnection(UPinHandle* OtherPinHandle)
{
	// Cut connection between these two nodes.
	RemoveConnection(OtherPinHandle);
	OtherPinHandle->RemoveConnection(this);
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

