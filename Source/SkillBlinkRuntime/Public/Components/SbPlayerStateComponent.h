// Copyright (c) Marko Petric & Yevhenii Selivanov

#pragma once

#include "Components/ActorComponent.h"

#include "SbPlayerStateComponent.generated.h"

/**
 * Represents the Player State in the SkillBlink module, where the Owner is Player State actor.
 * Is responsible for granting and removing the Blink ability, handling its cooldown, and its input.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SKILLBLINKRUNTIME_API USbPlayerStateComponent : public UActorComponent
{
	GENERATED_BODY()

	/*********************************************************************************************
	 * Public functions
	 ********************************************************************************************* */
public:
	/** Default constructor. */
	USbPlayerStateComponent();
	
	/** Returns Player State of this component. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "[SkillBlink]")
	class ABmrPlayerState* GetPlayerState() const;
	ABmrPlayerState& GetPlayerStateChecked() const;
	
	/*********************************************************************************************
	 * Main methods
	 ********************************************************************************************* */
public:
	/** Grants the Blink ability to the owner's ASC. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "[SkillBlink]")
	void GiveBlinkAbility();

	/** Clears the Blink ability from the owner's ASC. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "[SkillBlink]")
	void ClearBlinkAbility();
	
	/*********************************************************************************************
	 * Overrides
	 ********************************************************************************************* */
protected:
	/** Called when the owning Actor begins play or when the component is created if the Actor has already begun play. */
	virtual void BeginPlay() override;
	
	/** Clears all transient data created by this component. */
	virtual void OnUnregister() override;
};
