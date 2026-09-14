// Copyright (c) Marko Petric & Yevhenii Selivanov

#pragma once

#include "Abilities/GameplayAbility.h"

#include "SbBlinkAbility.generated.h"

/**
 * 
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
	void BroadcastBlinkResult(const FGameplayTag& FailureTag, const AActor* Instigator);
	
	/** Executes the appropriate Blink cue depending on the tag passed in. */
	void ExecuteBlinkCue(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTag& CueTag) const;

	/*********************************************************************************************
	 * Overrides
	 ********************************************************************************************* */
protected:
	/** Is overridden to prevent event-based activation if there is no cooldown GE set. */
	virtual bool ShouldAbilityRespondToEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* TriggerEventData) const override;
	
	/** Actually activate ability, do not call this directly. */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
