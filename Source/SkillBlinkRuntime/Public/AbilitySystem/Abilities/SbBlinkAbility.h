// Copyright (c) Marko Petric & Yevhenii Selivanov

#pragma once

#include "Abilities/GameplayAbility.h"

#include "SbBlinkAbility.generated.h"

/**
 * Handles teleporting (blinking) the player in the direction of the player forward vector if standing still, otherwise in input direction
 * Players can blink through obstacles or in open space
 * Ability is triggered by the SbGameplayTags::Event::BlinkActivated event
 */
UCLASS()
class SKILLBLINKRUNTIME_API USbBlinkAbility : public UGameplayAbility
{
	GENERATED_BODY()

	/*********************************************************************************************
	 * Main methods
	 ********************************************************************************************* */
protected:
	/** Broadcasts the Blink ability result after activation. */
	UFUNCTION(BlueprintCallable, Category = "[SkillBlink]")
	void BroadcastBlinkResult(const FGameplayTag& FailureTag, const AActor* Instigator);

	/** Executes the appropriate Blink cue depending on the tag passed in. */
	UFUNCTION(BlueprintCallable, Category = "[SkillBlink]")
	void ExecuteBlinkResultCue(const FGameplayAbilityActorInfo& ActorInfo, const FGameplayTag& CueTag) const;
	
	/** Adds the blink trail cue from the player's current location to the target cell, and removes it after a short delay. */
	UFUNCTION(BlueprintCallable, Category = "[SkillBlink]")
	void HandleBlinkTrailCue(const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilitySpecHandle& Handle, const struct FBmrCell& TargetCell);
	
	/** Spawns the portal effects at the player's current location and at the blink destination. */
	void SpawnBlinkPortals(const class ABmrPawn* AvatarPawn, const FBmrCell& TargetCell) const;
	
	/** Finds the farthest valid cell in the specified blink direction. */
	UFUNCTION(BlueprintCallable, Category = "[SkillBlink]")
	FBmrCell FindFarthestValidBlinkTargetCell(const ABmrPawn* AvatarPawn, const FVector& BlinkDirection, const FBmrCell& PlayerCell) const;
	
	/** Tries to find a blink target cell if blinking through a corner. */
	UFUNCTION(BlueprintCallable, Category = "[SkillBlink]")
	FBmrCell FindCornerBlinkTargetCell(const ABmrPawn* AvatarPawn, const FVector& BlinkDirection, const FBmrCell& PlayerCell) const;
	
	/** Finds the cell the player should blink to, or an invalid cell if there is no valid target. */
	UFUNCTION(BlueprintCallable, Category = "[SkillBlink]")
	FBmrCell FindBlinkTargetCell(const ABmrPawn* AvatarPawn, const FVector& BlinkDirection, const FBmrCell& PlayerCell) const;

	/** Finds a free cell near the player around the blink direction that is closest to the given target cell. */
	UFUNCTION(BlueprintCallable, Category = "[SkillBlink]")
	FBmrCell FindNearbyBlinkTargetCell(const FBmrCell& PlayerCell, const FVector& BlinkDirection, const FBmrCell& TargetCell) const;

	/*********************************************************************************************
	 * Overrides
	 ********************************************************************************************* */
protected:
	/** Is overridden to prevent event-based activation if there is no cooldown GE set. */
	virtual bool ShouldAbilityRespondToEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* TriggerEventData) const override;

	/** Actually activate ability, do not call this directly. */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
