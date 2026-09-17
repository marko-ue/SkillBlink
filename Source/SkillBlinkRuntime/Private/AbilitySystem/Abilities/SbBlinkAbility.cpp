// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "AbilitySystem/Abilities/SbBlinkAbility.h"

// Sb
#include "Data/SbDataAsset.h"
#include "SbGameplayTags.h"

// Bomber
#include "Actors/BmrPawn.h"
#include "Bomber.h"
#include "Components/BmrMoverComponent.h"
#include "Subsystems/GlobalMessageSubsystem.h"
#include "UtilityLibraries/BmrCellUtilsLibrary.h"

// UE
#include "GameplayCueManager.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbBlinkAbility)

// 1.5f is a bias that pushes the target vector far enough so SnapVectorOnLevel picks the cell ahead of the player
// It doesn't actually affect where the player is teleported, since the new cell's location is used directly when teleporting
static constexpr float BlinkSnapBias = 1.5f;

/*********************************************************************************************
 * Main methods
 ********************************************************************************************* */

// Broadcasts the Blink ability result after activation
void USbBlinkAbility::BroadcastBlinkResult(const FGameplayTag& FailureTag, const AActor* Instigator)
{
	FGameplayEventData EventData;
	EventData.EventTag = FailureTag;
	EventData.Instigator = Instigator;
	UGlobalMessageSubsystem::BroadcastGlobalMessage(EventData);
}

// Executes the appropriate Blink cue depending on the tag passed in
void USbBlinkAbility::ExecuteBlinkCue(const FGameplayAbilityActorInfo& ActorInfo, const FGameplayTag& CueTag) const
{
	if (ActorInfo.IsLocallyControlled())
	{
		const FGameplayCueParameters CueParams;
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(ActorInfo.AvatarActor.Get(), CueTag, CueParams);
	}
}

/*********************************************************************************************
 * Overrides
 ********************************************************************************************* */

// Is overridden to prevent event-based activation if there is no cooldown GE set
bool USbBlinkAbility::ShouldAbilityRespondToEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* TriggerEventData) const
{
	return Super::ShouldAbilityRespondToEvent(ActorInfo, TriggerEventData)
	       && ensureMsgf(GetCooldownGameplayEffect(), TEXT("ASSERT: [%i] %hs:\n'CooldownGE' is null!"), __LINE__, __FUNCTION__);
}

// Actually activate ability, do not call this directly
void USbBlinkAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const ABmrPawn* AvatarPawn = Cast<ABmrPawn>(ActorInfo->AvatarActor.Get());
	if (!ensureMsgf(AvatarPawn, TEXT("ASSERT: [%i] %hs:\n'AvatarPawn' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}

	UBmrMoverComponent* MoverComp = AvatarPawn->GetMoverComponent();
	if (!ensureMsgf(MoverComp, TEXT("ASSERT: [%i] %hs:\n'MoverComp' is null!"), __LINE__, __FUNCTION__))
	{
		return;
	}

	// Blink in the direction of current movement input if held, otherwise use actor forward vector
	const FMoverDefaultSyncState* SyncState = MoverComp->GetSyncState().SyncStateCollection.FindDataByType<FMoverDefaultSyncState>();
	const FVector InputIntent = SyncState ? SyncState->MoveDirectionIntent : FVector::ZeroVector;
	const FVector BlinkDirection = InputIntent.SizeSquared() > KINDA_SMALL_NUMBER
	                                   ? InputIntent.GetSafeNormal()
	                                   : AvatarPawn->GetActorForwardVector();

	// Location used to find the nearest grid cell to blink to
	const FVector BlinkTargetLocation = AvatarPawn->GetActorLocation() + BlinkDirection * (FBmrCell::CellSize * (BlinkSnapBias + USbDataAsset::Get().GetBlinkExtraTiles()));

	// Initializes the FBmrCell struct with a snap to the nearest cell in that blink target location
	const FBmrCell TargetCell = UBmrCellUtilsLibrary::SnapVectorOnLevel(BlinkTargetLocation);
	
	// If the player is on the same cell as the blink destination cell, fail the blink
	// This prevents the dash from succeeding if the player tries to blink out of bounds (map edge)
	const FBmrCell PlayerCell = UBmrCellUtilsLibrary::SnapActorOnLevel(AvatarPawn);
	if (TargetCell == PlayerCell)
	{
		BroadcastBlinkResult(SbGameplayTags::Event::BlinkFailed_InvalidCell, AvatarPawn);
		ExecuteBlinkCue(*ActorInfo, SbGameplayTags::GameplayCue::BlinkFailed);
		K2_EndAbility();
		return;
	}

	// Fail blink if the target cell is not valid
	if (!TargetCell.IsValid())
	{
		BroadcastBlinkResult(SbGameplayTags::Event::BlinkFailed_InvalidCell, AvatarPawn);
		ExecuteBlinkCue(*ActorInfo, SbGameplayTags::GameplayCue::BlinkFailed);
		K2_EndAbility();
		return;
	}

	// Fail blink if the target cell is occupied by a wall, box or bomb
	if (UBmrCellUtilsLibrary::IsCellHasAnyMatchingActor(TargetCell, TO_FLAG(EAT::Wall) | TO_FLAG(EAT::Box) | TO_FLAG(EAT::Bomb)))
	{
		BroadcastBlinkResult(SbGameplayTags::Event::BlinkFailed_Occupied, AvatarPawn);
		ExecuteBlinkCue(*ActorInfo, SbGameplayTags::GameplayCue::BlinkFailed);
		K2_EndAbility();
		return;
	}

	// Teleport (blink) the player to the blink target location
	MoverComp->TeleportToLocation(TargetCell.Location);
	BroadcastBlinkResult(SbGameplayTags::Event::BlinkSucceeded, AvatarPawn);
	ExecuteBlinkCue(*ActorInfo, SbGameplayTags::GameplayCue::BlinkSucceeded);

	// Spawn the portal niagara system at the player's current location and the target cell's location (the blink destination)
	if (UNiagaraSystem* PortalNiagaraSystem = USbDataAsset::Get().GetPortalNiagaraSystem())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PortalNiagaraSystem, AvatarPawn->GetActorLocation());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PortalNiagaraSystem, TargetCell.Location);
	}

	// Ability only commits its cooldown if the teleportation succeeded
	CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, false);

	K2_EndAbility();
}
