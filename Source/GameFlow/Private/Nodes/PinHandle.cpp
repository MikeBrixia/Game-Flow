
#include "Nodes/PinHandle.h"
#include "GameFlow.h"
#include "Nodes/GameFlowNode.h"

FPinConnectionInfo::FPinConnectionInfo()
{
}

FPinConnectionInfo::FPinConnectionInfo(const FName& InputPinName, UGameFlowNode* Node)
{
	this->DestinationPinName = InputPinName;
	this->DestinationObject = Node;
}

FPinHandle::FPinHandle()
{
	PinDirection = EGPD_MAX;
	bIsBreakpointEnabled = false;
}

FPinHandle::FPinHandle(FName PinName, UGameFlowNode* PinOwner, TEnumAsByte<EEdGraphPinDirection> PinDirection) : FPinHandle()
{
	this->PinName = PinName;
	this->PinOwner = PinOwner;
	this->PinDirection = PinDirection;
}

FPinHandle::FPinHandle(const FPinHandle& Other) : FPinHandle()
{
	this->PinName = Other.PinName;
	this->PinDirection = Other.PinDirection;
	this->PinOwner = Other.PinOwner;
	this->Connections = Other.Connections;
}

#if WITH_EDITOR

void FPinHandle::CreateConnection(FPinHandle& OtherPinHandle)
{
	if(CanCreateConnection(OtherPinHandle))
	{
		const FName PinFullName = GetFullPinName();
		const FName OtherPinFullName = OtherPinHandle.GetFullPinName();
		
		Connections.Add(OtherPinFullName, {OtherPinHandle.PinName, OtherPinHandle.PinOwner});
		OtherPinHandle.Connections.Add(PinFullName, {PinName, PinOwner});

		PinOwner->UpdatePinHandle(*this);
		OtherPinHandle.PinOwner->UpdatePinHandle(OtherPinHandle);
	}
}

void FPinHandle::CutConnection(FPinHandle& OtherPinHandle)
{
	Connections.Remove(OtherPinHandle.GetFullPinName());
	OtherPinHandle.Connections.Remove(GetFullPinName());
}

void FPinHandle::CutAllConnections()
{
	for(const auto& Pair : Connections)
	{
		const FPinConnectionInfo& ConnectionInfo = Pair.Value;
		// The opposite direction of this pin.
		const EEdGraphPinDirection Direction = this->PinDirection == EGPD_Input? EGPD_Output : EGPD_Input;
		FPinHandle ConnectedPin = ConnectionInfo.DestinationObject->GetPinByName(ConnectionInfo.DestinationPinName, Direction);
		
        // After having found the connected pin, cut the connection.
		CutConnection(ConnectedPin);
		
		// Finally we need to update both pins handle.
		PinOwner->UpdatePinHandle(*this);
		ConnectionInfo.DestinationObject->UpdatePinHandle(ConnectedPin);
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
	// Do not allow a connection between two pins on the same node.
	const bool bRecursiveConnection = PinOwner == OtherPinHandle.PinOwner;
	// True if the two handles do not have the same direction(e.g. Input pins can only connect to output pins and vice-versa).
	const bool bHaveDifferentDirections = PinDirection != OtherPinHandle.PinDirection;
	return bValidHandles && !bRecursiveConnection && bHaveDifferentDirections;
}

FName FPinHandle::GetFullPinName() const
{
	if(IsValidHandle())
	{
		const FString NodeName = PinOwner->GetName();
		return FName(NodeName + "." + PinName.ToString());
	}
	return EName::None;
}

#endif

