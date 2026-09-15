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
	 * Input
	 ********************************************************************************************* */
public:
	/** Returns the Blink input context. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "[SkillBlink]")
	FORCEINLINE class UBmrInputMappingContext* GetBlinkInputContext() const { return BlinkInputContext; }

protected:
	/** Input context for the Blink ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (BlueprintProtected, ShowOnlyInnerProperties))
	TObjectPtr<class UBmrInputMappingContext> BlinkInputContext = nullptr;
	
	/*********************************************************************************************
	 * Blink ability
	 ********************************************************************************************* */
public:
	/** Returns the Blink ability class. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "[SkillBlink]")
	FORCEINLINE TSubclassOf<class UGameplayAbility> GetBlinkAbilityClass() const { return BlinkAbilityClass; }
	
protected:
	/** The Blink ability class to grant to the player. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities", meta = (BlueprintProtected, ShowOnlyInnerProperties))
	TSubclassOf<UGameplayAbility> BlinkAbilityClass = nullptr;
	
	/*********************************************************************************************
	 * VFX
	 ********************************************************************************************* */
	
public:
	/** Returns the niagara system used by the Blink ability. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "[SkillBlink]")
	FORCEINLINE class UNiagaraSystem* GetPortalNiagaraSystem() const { return PortalNiagaraSystem; }
	
protected:
	/** Niagara system (portal) used by the Blink ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX", meta = (BlueprintProtected, ShowOnlyInnerProperties))
	TObjectPtr<UNiagaraSystem> PortalNiagaraSystem = nullptr;
};
