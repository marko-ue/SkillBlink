// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "AbilitySystem/Abilities/SbBlinkAbility.h"

// Bomber
#include "Actors/BmrPawn.h"
#include "Bomber.h"
#include "Components/BmrMoverComponent.h"
#include "UtilityLibraries/BmrCellUtilsLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbBlinkAbility)

// 1.5f is a bias that pushes the target vector far enough so SnapVectorOnLevel picks the cell ahead of the player
static constexpr float BlinkSnapBias = 1.5f;

/*********************************************************************************************
 * Overrides
 ********************************************************************************************* */

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
	const FVector BlinkTargetLocation = AvatarPawn->GetActorLocation() + BlinkDirection * (FBmrCell::CellSize * BlinkSnapBias);
	
	// Initializes the FBmrCell struct with a snap to the nearest cell in that blink target location
	const FBmrCell TargetCell = UBmrCellUtilsLibrary::SnapVectorOnLevel(BlinkTargetLocation);
	
	// End ability and allow another attempt if the target cell after blinking is not valid or if the target cell is occupied by a wall, box or bomb
	if (!TargetCell.IsValid() || UBmrCellUtilsLibrary::IsCellHasAnyMatchingActor(TargetCell, TO_FLAG(EAT::Wall) | TO_FLAG(EAT::Box) | TO_FLAG(EAT::Bomb)))
	{
		K2_EndAbility();
		return;
	}

	// Teleport (blink) the player to the blink target location
	MoverComp->TeleportToLocation(TargetCell.Location);

	// Ability only commits its cost if the teleportation succeeded
	// TODO: Replace with clearing the ability spec once the pickup for the ability is implemented
	CommitAbilityCost(Handle, ActorInfo, ActivationInfo);
    
	K2_EndAbility();
}
