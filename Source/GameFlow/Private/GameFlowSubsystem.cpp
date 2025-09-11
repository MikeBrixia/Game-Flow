// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFlowSubsystem.h"
#include "GameplayTagContainer.h"
#include "Engine/World.h"
#include "GameFramework/GameSession.h"
#include "Kismet/GameplayStatics.h"
#include "Nodes/World/GameFlowNode_WorldListener.h"

void UGameFlowSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	TArray<AActor*> WorldActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), WorldActors);
	for(const AActor* WorldActor : WorldActors)
	{
		UActorComponent* Component = WorldActor->GetComponentByClass(UGameFlowListener::StaticClass());
		if(Component != nullptr)
		{
			UGameFlowListener* ComponentListener = CastChecked<UGameFlowListener>(Component);
			RegisterListener(ComponentListener);
		}
	}
}

UGameFlowAsset* UGameFlowSubsystem::RegisterAssetInstance(UGameFlowAsset* Asset)
{
	UGameFlowAsset* AssetInstance = InstancedAssets.FindRef(Asset);
	// Create only unique asset instances.
	if(AssetInstance == nullptr)
	{
		AssetInstance = Asset->CreateInstance(this);
		InstancedAssets.Add(Asset->GetArchetype(), AssetInstance);
		
		// Listen for finish events. It is necessary to know when we need to unregister asset instance.
		AssetInstance->OnFinish.AddUObject(this, &UGameFlowSubsystem::UnregisterAssetInstance);
	}
	// Print the warning message both on the screen and in the log console.
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
				FString::Printf(TEXT("Asset %s has already been instanced inside %s scene/level and therefore could not be registered."), *Asset->GetName(),
					*GetWorld()->GetName()), true);
		}
		UE_LOG(LogGameSession, Warning, TEXT("%s has already been instanced inside %s scene/level and therefore could not be registered. Returning already instanced object"),
				   *Asset->GetName(), *GetWorld()->GetName());
	}

	return AssetInstance;
}

void UGameFlowSubsystem::UnregisterAssetInstance(UGameFlowAsset* AssetInstance)
{
	const UObject* InstanceArchetype = AssetInstance->GetArchetype();
	InstancedAssets.Remove(InstanceArchetype);
}

void UGameFlowSubsystem::RegisterListener(UGameFlowListener* Listener)
{
	if(Listener != nullptr)
	{
		Listeners.Add(Listener);
		OnListenerComponentRegistered.Broadcast(Listener);
	}
}

void UGameFlowSubsystem::UnregisterListener(UGameFlowListener* Listener)
{
	if(Listener != nullptr)
	{
		Listeners.Remove(Listener);
		OnListenerComponentUnregistered.Broadcast(Listener);
	}
}

void UGameFlowSubsystem::Execute(UGameFlowAsset* Asset, FName RootName)
{
	UGameFlowAsset* RuntimeAsset = RegisterAssetInstance(Asset);
    // Has the asset been registered successfully?
	if(RuntimeAsset != nullptr)
	{
		RuntimeAsset->Execute(RootName);
	}
}

TArray<UGameFlowAsset*> UGameFlowSubsystem::GetRunningFlows() const
{
	TArray<UGameFlowAsset*> RunningAssets;
	InstancedAssets.GenerateValueArray(RunningAssets);
	return RunningAssets;
}

UGameFlowAsset* UGameFlowSubsystem::GetRunningFlowByArchetype(UObject* Archetype) const
{
	return InstancedAssets.FindRef(Archetype);
}

TArray<UGameFlowListener*> UGameFlowSubsystem::GetListenersByGameplayTags(FGameplayTagContainer GameplayTag, EGameplayContainerMatchType MatchType) const
{
	TArray<UGameFlowListener*> QueriedListeners = Listeners.FilterByPredicate([=] (const UGameFlowListener* Listener)
	{
		const FGameplayTagContainer Tags = Listener->IdentityTags;
		return MatchType == EGameplayContainerMatchType::All? Tags.HasAll(GameplayTag) : Tags.HasAny(GameplayTag);
	});
	return QueriedListeners;
}

void UGameFlowSubsystem::NotifyListeners(FGameplayTagContainer GameplayTag, EGameplayContainerMatchType MatchType)
{
	TArray<UGameFlowListener*> QueriedListeners = GetListenersByGameplayTags(GameplayTag, MatchType);
	// Broadcast game flow event to all listeners.
	for(const UGameFlowListener* Listener : QueriedListeners)
	{
		if(Listener->OnReceiveGameFlowEvent.IsBound())
		{
			Listener->OnReceiveGameFlowEvent.Broadcast(GameplayTag);
		}
	}
}



