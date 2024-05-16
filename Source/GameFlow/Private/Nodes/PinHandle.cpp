#include "Nodes/PinHandle.h"

FPinHandle::FPinHandle()
{
	PinName = EName::None;
}

FPinHandle::FPinHandle(const FPinHandle& Other)
{
	this->Connections = Other.Connections;
	this->PinName = Other.PinName;
}

FPinHandle::FPinHandle(TMap<FName, UGameFlowNode*> Connections, FName PinName)
{
	this->Connections = Connections;
	this->PinName = PinName;
}

void FPinHandle::CreateConnection(FPinHandle& OtherPinHandle)
{
	// Do we have the permission to connect this two handles?
	if(CanCreateConnection(OtherPinHandle))
	{
		// First create a connection from this handle to the other...
		Connections.Add(OtherPinHandle.PinName, OtherPinHandle.PinOwner);
		// ... then create a connection in the opposite direction.
		OtherPinHandle.Connections.Add(PinName, PinOwner);
	}
}

bool FPinHandle::IsValidHandle() const
{
	// A valid pin name should not be 'None' or whitespaces only.
	const bool bIsValidName = !PinName.IsNone() && PinName.IsValid()
	       && !PinName.ToString().IsEmpty();
	return bIsValidName && PinOwner != nullptr;
}

bool FPinHandle::CanCreateConnection(const FPinHandle& OtherPinHandle) const
{
	const bool bValidHandles = IsValidHandle() && OtherPinHandle.IsValidHandle();
	const bool bValidOwners = PinOwner != nullptr && OtherPinHandle.PinOwner != nullptr;
	const bool bRecursiveConnection = PinOwner == OtherPinHandle.PinOwner;
	return bValidHandles && bValidOwners && !bRecursiveConnection;
}
