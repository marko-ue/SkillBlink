// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "AbilitySystem/Abilities/SbBlinkAbility.h"

// Sb
#include "Data/SbDataAsset.h"
#include "SbGameplayTags.h"

// Bomber
#include "Actors/BmrPawn.h"
#include "Components/BmrMoverComponent.h"
#include "Subsystems/GlobalMessageSubsystem.h"
#include "UtilityLibraries/BmrCellUtilsLibrary.h"

// UE
#include "AbilitySystemComponent.h"
#include "GameplayCueManager.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbBlinkAbility)

// This value is used directly if the ShouldBlinkRangeBeInfinite CVar is set to true to make the Blink have infinite range
static constexpr int32 BlinkInfiniteRange = 8;

// Corner sweep settings: how far along the ray to sweep, how far to each side to sample, the distance between samples, and a constant to find the target tile consistently
static constexpr float BlinkCornerSweepRange = 1.5f;
static constexpr float BlinkCornerSweepRadius = 0.2f;
static constexpr float BlinkCornerSweepStep = 0.1f;
static constexpr float BlinkCornerBeyondDistance = 0.5;

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

// Finds the farthest valid cell in the specified blink direction
FBmrCell USbBlinkAbility::FindFarthestValidBlinkCell(const ABmrPawn* AvatarPawn, const FVector& BlinkDirection, const FBmrCell& PlayerCell) const
{
	for (int32 Step = BlinkInfiniteRange; Step >= 1; --Step)
	{
		const FVector CurrentLocation = AvatarPawn->GetActorLocation() + BlinkDirection * (FBmrCell::CellSize * Step);
		const FBmrCell FarthestValidCell = UBmrCellUtilsLibrary::SnapVectorOnLevel(CurrentLocation);

		if (UBmrCellUtilsLibrary::IsCellExistsOnLevel(FarthestValidCell)
		   && !UBmrCellUtilsLibrary::IsCellBlocked(FarthestValidCell)
		   && FarthestValidCell != PlayerCell)
		{
			return FarthestValidCell;
		}
	}

	return FBmrCell::InvalidCell;
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

	const FBmrCell PlayerCell = UBmrCellUtilsLibrary::SnapActorOnLevel(AvatarPawn);

	// Target cell whose location will be passed in for the blink location
	FBmrCell TargetCell = FBmrCell::InvalidCell;
	bool bEncounteredObstacle = false;

	/* 
	 * Corner sweep: a very small sweep along the start of the blink ray that samples slightly to both sides of it
	 * If both sides land on different blocked cells, the ray is squeezing through a corner, so blink to the free cell right beyond it
	 */
	
	// The blink direction flattened to the ground plane
	const FVector RayDirection = FVector(BlinkDirection.X, BlinkDirection.Y, 0.f).GetSafeNormal();
	
	// A vector perpendicular to the ray used to probe the cells to each side of it
	const FVector SideOffset = FVector(-RayDirection.Y, RayDirection.X, 0.f) * (FBmrCell::CellSize * BlinkCornerSweepRadius);
	
	// How far the sample point moves forward along the ray on each iteration
	const float SweepStepDistance = FBmrCell::CellSize * BlinkCornerSweepStep;
	
	// How far along the ray the sweep goes in total, kept short so far away gaps can't override a nearer target
	const float SweepMaxDistance = FBmrCell::CellSize * BlinkCornerSweepRange;
	
	// Starts one step ahead of the player and moves forward until the max sweep distance is reached
	for (float SweepDistance = SweepStepDistance; SweepDistance <= SweepMaxDistance; SweepDistance += SweepStepDistance)
	{
		// The sample point lies on the ray, starting at 0.1 cells ahead of the player
		// Cells A and B are the cells to the left and right of the sample point on the ray
		const FVector SamplePoint = AvatarPawn->GetActorLocation() + RayDirection * SweepDistance;
		const FBmrCell SideCellA = UBmrCellUtilsLibrary::SnapVectorOnLevel(SamplePoint + SideOffset);
		const FBmrCell SideCellB = UBmrCellUtilsLibrary::SnapVectorOnLevel(SamplePoint - SideOffset);

		// The probes must differ on both axes to be a corner. Also accounts for when probes are on the player's tile
		if (SideCellA.Location.X == SideCellB.Location.X || SideCellA.Location.Y == SideCellB.Location.Y)
		{
			continue;
		}

		// Both side cells exist and are blocked, so the ray is passing through a gap between two obstacles
		if (UBmrCellUtilsLibrary::IsCellExistsOnLevel(SideCellA) && UBmrCellUtilsLibrary::IsCellBlocked(SideCellA)
			&& UBmrCellUtilsLibrary::IsCellExistsOnLevel(SideCellB) && UBmrCellUtilsLibrary::IsCellBlocked(SideCellB))
		{
			// The beyond cell is half a cell past the gap. If it exists, is free and is not the player cell, it becomes the blink target
			const FBmrCell BeyondCell = UBmrCellUtilsLibrary::SnapVectorOnLevel(SamplePoint + RayDirection * (FBmrCell::CellSize * BlinkCornerBeyondDistance));
			if (UBmrCellUtilsLibrary::IsCellExistsOnLevel(BeyondCell)
				&& !UBmrCellUtilsLibrary::IsCellBlocked(BeyondCell)
				&& BeyondCell != PlayerCell)
			{
				TargetCell = BeyondCell;
			}
			// Stop at the first gap found, even if the beyond cell was unusable, so the sweep never looks past the nearest gap
			// If TargetCell is still invalid here, the normal cell search below takes over
			break;
		}
	}

	// Goes through all cells ahead of the blink direction, only if the corner sweep didn't find a target
	if (!TargetCell.IsValid())
	{
		for (int32 Step = 1; Step <= BlinkInfiniteRange; ++Step)
		{
			// Candidate values start off from the current location and cell
			const FVector CurrentLocation = AvatarPawn->GetActorLocation() + BlinkDirection * (FBmrCell::CellSize * Step);
			const FBmrCell CandidateCell = UBmrCellUtilsLibrary::SnapVectorOnLevel(CurrentLocation);

			// If the candidate cell is not valid, or it's the same as the player's current cell, return
			if (!UBmrCellUtilsLibrary::IsCellExistsOnLevel(CandidateCell) || CandidateCell == PlayerCell)
			{
				BroadcastBlinkResult(SbGameplayTags::Event::BlinkFailed_InvalidCell, AvatarPawn);
				ExecuteBlinkCue(*ActorInfo, SbGameplayTags::GameplayCue::BlinkFailed);
				K2_EndAbility();
				return;
			}

			// Keep looking for empty cells ahead if there is an obstacle on the current checked cell
			if (UBmrCellUtilsLibrary::IsCellBlocked(CandidateCell))
			{
				bEncounteredObstacle = true;
				continue;
			}

			// The target cell becomes the first free cell after obstacle(s) 
			if (bEncounteredObstacle)
			{
				TargetCell = CandidateCell;
				break;
			}
		}
	}

	// If there were no obstacles, blink to the edge cell in that direction
	if (!TargetCell.IsValid() && !bEncounteredObstacle)
	{
		TargetCell = FindFarthestValidBlinkCell(AvatarPawn, BlinkDirection, PlayerCell);
	}
	
	// If there is no free cell after an encountered obstacle (obstacle is at the edge of the map), blink to the free cell right before the obstacle
	if (!TargetCell.IsValid() && bEncounteredObstacle)
	{
		TargetCell = FindFarthestValidBlinkCell(AvatarPawn, BlinkDirection, PlayerCell);
	}
	
	// [?] TODO: Random cell fallback if no valid cell
	
	// Don't blink if the target cell is invalid (a valid cell was never found)
	// This should never happen in practice due to a random cell fallback, but is here as a preventative measure
	if (TargetCell == FBmrCell::InvalidCell)
	{
		BroadcastBlinkResult(SbGameplayTags::Event::BlinkFailed_InvalidCell, AvatarPawn);
		ExecuteBlinkCue(*ActorInfo, SbGameplayTags::GameplayCue::BlinkFailed);
		K2_EndAbility();
		return;
	}
	
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	
	// Add cue for the blink trail
	FGameplayCueParameters Params;
	Params.Normal = AvatarPawn->GetActorLocation();
	Params.Location = TargetCell.Location;
	ASC->AddGameplayCue(SbGameplayTags::GameplayCue::BlinkTrail, Params);

	// Teleport (blink) the player to the blink target location
	MoverComp->TeleportToLocation(TargetCell.Location);
	BroadcastBlinkResult(SbGameplayTags::Event::BlinkSucceeded, AvatarPawn);
	ExecuteBlinkCue(*ActorInfo, SbGameplayTags::GameplayCue::BlinkSucceeded);
	
	// Remove cue for the blink trail after a short delay to allow the trail to move itself to the new location
	FTimerHandle TrailTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TrailTimerHandle, [this]()
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveGameplayCue(SbGameplayTags::GameplayCue::BlinkTrail);
	}, 0.2f, false);

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
