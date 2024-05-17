
#include "Nodes/PinHandle.h"

FGameFlowPinNodePair::FGameFlowPinNodePair()
{
	this->OtherPinName = EName::None;
	this->Node = nullptr;
}

FGameFlowPinNodePair::FGameFlowPinNodePair(const FName& InputPinName, UGameFlowNode* Node)
{
	this->OtherPinName = InputPinName;
	this->Node = Node;
}


FPinHandle::FPinHandle()
{
	PinName = EName::None;
	bIsBreakpointEnabled = true;
	PinDirection = EGPD_MAX;
}

FPinHandle::FPinHandle(FName PinName) : FPinHandle()
{
	this->PinName = PinName;
}

FPinHandle::FPinHandle(const FPinHandle& Other) : FPinHandle()
{
	this->Connections = Other.Connections;
	this->PinName = Other.PinName;
}

FPinHandle::FPinHandle(TArray<FGameFlowPinNodePair> Connections, FName PinName) : FPinHandle()
{
	this->Connections = Connections;
	this->PinName = PinName;
}

#if WITH_EDITOR

void FPinHandle::CreateConnection(FPinHandle& OtherPinHandle)
{
	// Do we have the permission to connect this two handles?
	if(CanCreateConnection(OtherPinHandle))
	{
		// First create a connection from this handle to the other...
		Connections.Add({OtherPinHandle.PinName, OtherPinHandle.PinOwner});
		// ... then create a connection in the opposite direction.
		OtherPinHandle.Connections.Add({PinName, PinOwner});
	}
}

void FPinHandle::CutConnection(FPinHandle& OtherPinHandle)
{
	Connections.Remove({OtherPinHandle.PinName, OtherPinHandle.PinOwner});
	OtherPinHandle.Connections.Remove({PinName, PinOwner});
}

void FPinHandle::CutAllConnections()
{
	Connections.Empty();
}

void FPinHandle::RenamePin(FName NewPinName)
{
	switch(PinDirection)
	{
	case EGPD_Input:
		break;
		
	case EGPD_Output:
		break;
		
	default: break;
	}
}

bool FPinHandle::IsValidHandle() const
{
	return IsValidPinName() && PinOwner != nullptr;
}

bool FPinHandle::IsValidPinName() const
{
	// A valid pin name should not be 'None' or whitespaces only.
	return !PinName.IsNone() && PinName.IsValid()
		   && !PinName.ToString().IsEmpty();
}

bool FPinHandle::CanCreateConnection(const FPinHandle& OtherPinHandle) const
{
	const bool bValidHandles = IsValidHandle() && OtherPinHandle.IsValidHandle();
	const bool bValidOwners = PinOwner != nullptr && OtherPinHandle.PinOwner != nullptr;
	const bool bRecursiveConnection = PinOwner == OtherPinHandle.PinOwner;
	return bValidHandles && bValidOwners && !bRecursiveConnection;
}

#endif