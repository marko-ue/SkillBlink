// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "Components/SbPlayerStateComponent.h"

// Bomber
#include "GameFramework/BmrPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbPlayerStateComponent)

/*********************************************************************************************
 * Public functions
 ********************************************************************************************* */

// Sets default values for this component's properties
USbPlayerStateComponent::USbPlayerStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Returns Player State of this component
ABmrPlayerState* USbPlayerStateComponent::GetPlayerState() const
{
	return Cast<ABmrPlayerState>(GetOwner());
}

ABmrPlayerState& USbPlayerStateComponent::GetPlayerStateChecked() const
{
	ABmrPlayerState* MyPlayerState = GetPlayerState();
	checkf(MyPlayerState, TEXT("%s: 'MyPlayerState' is null"), *FString(__FUNCTION__));
	return *MyPlayerState;
}

// Called when the owning Actor begins play or when the component is created if the Actor has already begun play
void USbPlayerStateComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called when the component is unregistered, used to clean up resources
void USbPlayerStateComponent::OnUnregister()
{
	Super::OnUnregister();
}
