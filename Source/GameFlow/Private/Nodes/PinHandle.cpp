
#include "Nodes/PinHandle.h"
#include "GameFlow.h"
#include "Config/GameFlowSettings.h"
#include "Nodes/GameFlowNode.h"

UPinHandle::UPinHandle()
{
	PinDirection = EGPD_MAX;
	bIsBreakpointEnabled = false;
}

void UPinHandle::TriggerPin()
{
	// Only output pins can broadcast events.
	if(bIsOutput)
	{
		for(const auto& Pair : Connections)
		{
			UPinHandle* PinHandle = Pair.Value;
			PinHandle->TriggerPin();
		}
#if WITH_EDITOR
		bIsActive = true;
		ActivatedElapsedTime = UGameFlowSettings::Get()->WireHighlightDuration;
#endif
	}
	// Input pins instead will directly try to execute the owner.
	else
	{
		PinOwner->TryExecute(PinName);
	}
}

#if WITH_EDITOR

UPinHandle* UPinHandle::CreatePinHandle(FName PinName, UGameFlowNode* PinOwner, EEdGraphPinDirection PinDirection)
{
	const FString NodeName = PinOwner->GetName();
	const FName FullPinName = FName(NodeName + "." + PinName.ToString());
	UPinHandle* NewPinHandle = NewObject<UPinHandle>(PinOwner, FullPinName);
	NewPinHandle->PinDirection = PinDirection;
	NewPinHandle->bIsOutput = PinDirection == EGPD_Output;
	NewPinHandle->PinName = PinName;
	NewPinHandle->PinOwner = PinOwner;
	return NewPinHandle;
}

void UPinHandle::CreateConnection(UPinHandle* OtherPinHandle)
{
	if(CanCreateConnection(OtherPinHandle))
	{
		const FName PinFullName = GetFName();
		const FName OtherPinFullName = OtherPinHandle->GetFName();
		
		// Register editor connection info.
		Connections.Add(OtherPinFullName, OtherPinHandle);
		OtherPinHandle->Connections.Add(PinFullName, this);
	}
}

void UPinHandle::CutConnection(UPinHandle* OtherPinHandle)
{
	const FName PinFullName = OtherPinHandle->GetFName();

	// Cut connection between these two nodes.
	Connections.Remove(PinFullName);
	OtherPinHandle->Connections.Remove(PinFullName);
}

void UPinHandle::CutAllConnections()
{
	for(const auto& Pair : Connections)
	{
		UPinHandle* OtherPinHandle = Pair.Value;
        // After having found the connected pin, cut the connection.
		CutConnection(OtherPinHandle);
	}
}

bool UPinHandle::IsValidHandle() const
{
	return IsValidPinName() && PinOwner != nullptr;
}

bool UPinHandle::IsValidPinName() const
{
	// A valid pin name should not be 'None' or whitespaces only.
	return !PinName.IsNone() && PinName.IsValid()
		   && !PinName.ToString().IsEmpty();
}

bool UPinHandle::CanCreateConnection(const UPinHandle* OtherPinHandle) const
{
	const bool bValidHandles = IsValidHandle() && OtherPinHandle->IsValidHandle();
	// Do not allow a connection between two pins on the same node.
	const bool bRecursiveConnection = PinOwner == OtherPinHandle->PinOwner;
	// True if the two handles do not have the same direction(e.g. Input pins can only connect to output pins and vice-versa).
	const bool bHaveDifferentDirections = PinDirection != OtherPinHandle->PinDirection;
	return bValidHandles && !bRecursiveConnection && bHaveDifferentDirections;
}

#endif

