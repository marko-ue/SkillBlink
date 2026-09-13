// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "Components/SbPlayerStateComponent.h"

// Sb
#include "Data/SbDataAsset.h"

// Bomber
#include "DalSubsystem.h"
#include "GameFramework/BmrPlayerState.h"

// UE
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"

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

/*********************************************************************************************
 * Main methods
 ********************************************************************************************* */

void USbPlayerStateComponent::GiveBlinkAbility()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Lambda that gives the Blink ability when the data asset becomes valid
	UDalSubsystem::Get().ListenForDataAsset<USbDataAsset>(this, [this](const USbDataAsset& DA)
	{
		UAbilitySystemComponent& ASC = GetPlayerStateChecked().GetAbilitySystemComponentChecked();
		const FGameplayAbilitySpec AbilitySpec(DA.GetBlinkAbilityClass());
		ASC.GiveAbility(AbilitySpec);
	});
}

void USbPlayerStateComponent::ClearBlinkAbility()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	UAbilitySystemComponent& ASC = GetPlayerStateChecked().GetAbilitySystemComponentChecked();

	const USbDataAsset* DataAsset = UDalSubsystem::GetDataAsset<USbDataAsset>();
	const FGameplayAbilitySpec* Spec = DataAsset ? ASC.FindAbilitySpecFromClass(DataAsset->GetBlinkAbilityClass()) : nullptr;
	if (Spec)
	{
		ASC.ClearAbility(Spec->Handle);
	}
}

// Called when the owning Actor begins play or when the component is created if the Actor has already begun play
void USbPlayerStateComponent::BeginPlay()
{
	Super::BeginPlay();
	
	GiveBlinkAbility();
}

// Called when the component is unregistered, used to clean up resources
void USbPlayerStateComponent::OnUnregister()
{
	ClearBlinkAbility();
	
	Super::OnUnregister();
}
