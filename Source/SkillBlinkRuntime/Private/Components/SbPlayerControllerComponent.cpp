// Copyright (c) Yevhenii Selivanov

#include "Components/SbPlayerControllerComponent.h"

// Bomber
#include "Controllers/BmrPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbPlayerControllerComponent)

// Sets default values for this component's properties
USbPlayerControllerComponent::USbPlayerControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Returns Player Controller of this component
ABmrPlayerController* USbPlayerControllerComponent::GetPlayerController() const
{
	return GetOwner<ABmrPlayerController>();
}

ABmrPlayerController& USbPlayerControllerComponent::GetPlayerControllerChecked() const
{
	ABmrPlayerController* MyPlayerController = GetPlayerController();
	checkf(MyPlayerController, TEXT("%s: 'MyPlayerController' is null"), *FString(__FUNCTION__));
	return *MyPlayerController;
}

/*********************************************************************************************
 * Main methods
 ********************************************************************************************* */

// Sets up the input context for SkillBlink for the player controller
void USbPlayerControllerComponent::SetupBlinkInputContext() const
{
	//GetPlayerControllerChecked().SetupInputContexts()
}

// Removes the input context for SkillBlink from the player controller
void USbPlayerControllerComponent::RemoveBlinkInputContextAndActions() const
{
	//GetPlayerControllerChecked().RemoveInputContexts()
}

/*********************************************************************************************
 * Overrides
 ********************************************************************************************* */

// Called when the owning Actor begins play or when the component is created if the Actor has already begun play
void USbPlayerControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetPlayerControllerChecked().IsLocalController())
	{
		SetupBlinkInputContext();
	}
}

// Clears all transient data created by this component
void USbPlayerControllerComponent::OnUnregister()
{
	if (GetPlayerControllerChecked().IsLocalController())
	{
		SetupBlinkInputContext();
	}
	
	Super::OnUnregister();
}

