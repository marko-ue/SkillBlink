// Copyright (c) Marko Petric & Yevhenii Selivanov

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "SbPlayerControllerComponent.generated.h"

class ABmrPlayerController;
/**
 * Represents the Player Controller in the SkillBlink module, where the Owner is Player Controller actor.
 * Is responsible for managing ability inputs.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SKILLBLINKRUNTIME_API USbPlayerControllerComponent : public UActorComponent
{
	GENERATED_BODY()

	/*********************************************************************************************
	 * Public functions
	 ********************************************************************************************* */
public:
	/** Default constructor. */
	USbPlayerControllerComponent();
	
	/** Returns Player Controller of this component. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "[SkillBlink]")
	ABmrPlayerController* GetPlayerController() const;
	ABmrPlayerController& GetPlayerControllerChecked() const;
	
	/*********************************************************************************************
	 * Main methods
	 ********************************************************************************************* */
protected:
	/** Sets up the input context for SkillBlink for the player controller. */
	void SetupBlinkInputContext() const;

	/** Removes the input context for SkillBlink from the player controller. */
	void RemoveBlinkInputContextAndActions() const;

	/*********************************************************************************************
	 * Overrides
	 ********************************************************************************************* */
protected:
	/** Called when the owning Actor begins play or when the component is created if the Actor has already begun play. */
	virtual void BeginPlay() override;

	/** Clears all transient data created by this component */
	virtual void OnUnregister() override;
};
