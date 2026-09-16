// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "Components/SbPlayerStateComponent.h"

// Sb
#include "Data/SbDataAsset.h"
#include "SbGameplayTags.h"

// Bomber
#include "DalSubsystem.h"
#include "GameFramework/BmrPlayerState.h"
#include "Subsystems/GlobalMessageSubsystem.h"

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

// Gives the Blink ability to the owner's ASC
void USbPlayerStateComponent::GiveBlinkAbility()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Lambda that gives the Blink ability and adds the Blink aura when the data asset becomes valid
	UDalSubsystem::Get().ListenForDataAsset<USbDataAsset>(this, [this](const USbDataAsset& DA)
	{
		UAbilitySystemComponent& ASC = GetPlayerStateChecked().GetAbilitySystemComponentChecked();

		if (GetOwner()->HasAuthority())
		{
			const FGameplayAbilitySpec AbilitySpec(DA.GetBlinkAbilityClass());
			ASC.GiveAbility(AbilitySpec);
		}

		// Adds the aura gameplay cue for the Blink ability
		ASC.AddGameplayCue(SbGameplayTags::GameplayCue::BlinkAura, ASC.MakeEffectContext());

		// Registers a listener for the Blink cooldown tag to hide/show the aura
		ASC.RegisterGameplayTagEvent(SbGameplayTags::GameplayEffect::BlinkCooldown, EGameplayTagEventType::NewOrRemoved)
		    .AddUObject(this, &ThisClass::OnCooldownTagChanged);
	});
}

// Clears the Blink ability from the owner's ASC
void USbPlayerStateComponent::ClearBlinkAbility()
{
	UAbilitySystemComponent& ASC = GetPlayerStateChecked().GetAbilitySystemComponentChecked();

	if (GetOwner()->HasAuthority())
	{
		const USbDataAsset* DataAsset = UDalSubsystem::GetDataAsset<USbDataAsset>();
		const FGameplayAbilitySpec* Spec = DataAsset ? ASC.FindAbilitySpecFromClass(DataAsset->GetBlinkAbilityClass()) : nullptr;
		if (Spec)
		{
			ASC.ClearAbility(Spec->Handle);
		}
	}

	// Removes the aura gameplay cue for the Blink ability
	ASC.RemoveGameplayCue(SbGameplayTags::GameplayCue::BlinkAura);

	// Unregisters the Blink cooldown gameplay tag event
	ASC.RegisterGameplayTagEvent(SbGameplayTags::GameplayEffect::BlinkCooldown, EGameplayTagEventType::NewOrRemoved)
	    .RemoveAll(this);
}

// Broadcasts the Blink ability activation event when input is started
void USbPlayerStateComponent::OnBlinkInputStarted()
{
	FGameplayEventData EventData;
	EventData.EventTag = SbGameplayTags::Event::BlinkActivated;
	EventData.Instigator = GetOwner();
	UGlobalMessageSubsystem::BroadcastGlobalMessage(EventData);
}

/*********************************************************************************************
 * Overrides
 ********************************************************************************************* */

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
	
	UGlobalMessageSubsystem::ClearCachedMessages(SbGameplayTags::Event::BlinkActivated);
	UGlobalMessageSubsystem::StopListeningForAllGlobalMessages(this);

	Super::OnUnregister();
}

/*********************************************************************************************
 * Events
 ********************************************************************************************* */

// Called when the cooldown tag for the Blink ability changes (when it goes on/off cooldown)
// TODO: Remove once pickup is implemented, cooldown won't exist since it will be a one-time use ability
void USbPlayerStateComponent::OnCooldownTagChanged_Implementation(struct FGameplayTag Tag, int32 NewCount)
{
	UAbilitySystemComponent& ASC = GetPlayerStateChecked().GetAbilitySystemComponentChecked();

	if (NewCount > 0)
	{
		// Cooldown started, hide the aura
		ASC.RemoveGameplayCue(SbGameplayTags::GameplayCue::BlinkAura);
	}
	else
	{
		// Cooldown ended, show the aura
		ASC.AddGameplayCue(SbGameplayTags::GameplayCue::BlinkAura, ASC.MakeEffectContext());
	}
}
