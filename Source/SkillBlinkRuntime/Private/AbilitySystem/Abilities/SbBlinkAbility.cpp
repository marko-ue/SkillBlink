// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "AbilitySystem/Abilities/SbBlinkAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbBlinkAbility)

// Actually activate ability, do not call this directly
void USbBlinkAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}