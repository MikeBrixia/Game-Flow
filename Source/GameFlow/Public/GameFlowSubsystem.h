// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlowAsset.h"
#include "GameFlowListener.h"
#include "UObject/Object.h"
#include "GameFlowSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnListenerComponentRegistered, UGameFlowListener*, ListenerComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnListenerComponentUnregistered, UGameFlowListener*, ListenerComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTagAdded, UGameFlowListener*, ListenerComponent, FGameplayTag, NewTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTagRemoved, UGameFlowListener*, ListenerComponent, FGameplayTag, RemovedTag);

/**
 * Game Flow singleton which handles execution and lifetime of
 * game flow assets inside the game.
 */
UCLASS(NotBlueprintable)
class GAMEFLOW_API UGameFlowSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:

	/** All the currently executing game flow assets inside this world. */
	UPROPERTY()
	TMap<UObject*, UGameFlowAsset*> InstancedAssets;
	
	/** Instanced assets which shares the lifetime of the world. */
	UPROPERTY()
	TArray<UGameFlowAsset*> WorldInstancedAssets;
	
	/** All the game flow listeners inside the world. */
	UPROPERTY()
	TArray<UGameFlowListener*> Listeners;

public:

	FOnListenerComponentRegistered OnListenerComponentRegistered;
	FOnListenerComponentUnregistered OnListenerComponentUnregistered;
	FOnTagAdded OnGameplayTagAdded;
	FOnTagRemoved OnGameplayTagRemoved;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UGameFlowAsset* RegisterAssetInstance(UGameFlowAsset* Asset);
	void UnregisterAssetInstance(UGameFlowAsset* AssetInstance);
	
	void RegisterListener(UGameFlowListener* Listener);
	void UnregisterListener(UGameFlowListener* Listener);
	
    /** Execute a specific root on a given game flow asset.
     * @param Asset The source asset blueprint
     * @param RootName The name of the root to execute, defaults to "Start" node.
     */
	UFUNCTION(BlueprintCallable, Category="Game Flow")
	void Execute(UGameFlowAsset* Asset, FName RootName = "Start");
	
	/** Get all the currently running game flow assets inside the level. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Game Flow")
	TArray<UGameFlowAsset*> GetRunningFlows() const;

	/**
	 * Get all listeners with matching gameplay tags.
	 * @param GameplayTag The tags used to search for the listeners.
	 * @param MatchType Matching tag strategy.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Game Flow")
	TArray<UGameFlowListener*> GetListenersByGameplayTags(FGameplayTagContainer GameplayTag, EGameplayContainerMatchType MatchType) const;

	/** Notify about a game flow event all component listeners with matching gameplay tag. */
	UFUNCTION(BlueprintCallable, Category="Game Flow")
	void NotifyListeners(FGameplayTagContainer GameplayTag, EGameplayContainerMatchType MatchType);
};
