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
	 * Overrides
	 ********************************************************************************************* */
protected:
	/** Actually activate ability, do not call this directly. */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
