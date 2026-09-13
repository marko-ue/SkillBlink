// Copyright (c) Marko Petric & Yevhenii Selivanov

#pragma once

#include "DalPrimaryDataAsset.h"

#include "SbDataAsset.generated.h"

/**
 * Contains common data of the SkillBlink plugin to be tweaked.
 */
UCLASS()
class SKILLBLINKRUNTIME_API USbDataAsset : public UDalPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	/** Returns this Data Asset, is checked and wil crash if can't be obtained, e.g: when is not set. */
	static const USbDataAsset& Get();
	
	/*********************************************************************************************
	 * Blink ability
	 ********************************************************************************************* */
public:
	/** Returns the Blink ability class. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "[SkillDash]")
	FORCEINLINE TSubclassOf<class UGameplayAbility> GetBlinkAbilityClass() const { return BlinkAbilityClass; }
	
protected:
	/** The Blink ability class to grant to the player. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities", meta = (BlueprintProtected, ShowOnlyInnerProperties))
	TSubclassOf<UGameplayAbility> BlinkAbilityClass = nullptr;
};
