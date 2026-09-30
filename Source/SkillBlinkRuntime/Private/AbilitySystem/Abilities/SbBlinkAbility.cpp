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

// Const used in for loops to search for all tiles in the blink direction
static constexpr int32 BlinkTileSearchAmount = 8;

// Corner sweep settings: how far along the ray to sweep (if capped), how far to each side to sample, the distance between samples, and a constant to find the target tile consistently
static constexpr float BlinkCornerSweepRange = 1.5f;
static constexpr float BlinkCornerSweepRadius = 0.2f;
static constexpr float BlinkCornerSweepStep = 0.1f;
static constexpr float BlinkCornerBeyondDistance = 0.5f;

// How many cells around the player to search when a blink would only move a single cell, and not over an obstacle
static constexpr int32 BlinkNearbySearchRadius = 3;

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
	for (int32 Step = BlinkTileSearchAmount; Step >= 1; --Step)
	{
		const FVector CurrentLocation = AvatarPawn->GetActorLocation() + BlinkDirection * (FBmrCell::CellSize * Step);
		const FBmrCell FarthestValidCell = UBmrCellUtilsLibrary::SnapVectorOnLevel(CurrentLocation);

		// The farthest valid cell is one that exists, is not blocked, and is not the player cell
		if (UBmrCellUtilsLibrary::IsCellExistsOnLevel(FarthestValidCell)
		   && !UBmrCellUtilsLibrary::IsCellBlocked(FarthestValidCell)
		   && FarthestValidCell != PlayerCell)
		{
			return FarthestValidCell;
		}
	}

	return FBmrCell::InvalidCell;
}

// Tries to find a blink target cell if blinking through a corner
FBmrCell USbBlinkAbility::FindCornerBlinkCell(const ABmrPawn* AvatarPawn, const FVector& BlinkDirection, const FBmrCell& PlayerCell) const
{
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

	// How far along the ray the sweep goes. Starts short so far away gaps can't override a nearer target, and extends whenever a corner is found
	float SweepMaxDistance = FBmrCell::CellSize * BlinkCornerSweepRange;

	// Should corner blinks should chain through consecutive corners
	const bool bChainThroughCorners = USbDataAsset::Get().ShouldBlinkChainThroughCorners();

	// Target cell whose location will be passed in for the blink location (if any)
	FBmrCell TargetCell = FBmrCell::InvalidCell;

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

				// With chaining disabled, the first corner is the final target cell
				if (!bChainThroughCorners)
				{
					break;
				}

				// Keep sweeping further to check if there are additional corners in the same diagonal, and use the furthest one as the target cell
				SweepMaxDistance = FMath::Min(SweepDistance + FBmrCell::CellSize * BlinkCornerSweepRange, FBmrCell::CellSize * BlinkTileSearchAmount);
				continue;
			}

			// Stop at the first gap whose beyond cell is unusable, keeping any target found from earlier corners
			// If the target is still invalid here, the normal cell search takes over
			break;
		}
	}

	return TargetCell;
}

// Finds a free cell near the player around the blink direction that is closest to the given target cell
FBmrCell USbBlinkAbility::FindNearbyBlinkCell(const FBmrCell& PlayerCell, const FVector& BlinkDirection, const FBmrCell& TargetCell) const
{
	FBmrCell NearbyCell = FBmrCell::InvalidCell;
	double BestDistanceSquared = TNumericLimits<double>::Max();
	
	// Finds offsets in both ways depending on the search radius, and loops until it checks all of them
	for (int32 OffsetY = -BlinkNearbySearchRadius; OffsetY <= BlinkNearbySearchRadius; ++OffsetY)
	{
		for (int32 OffsetX = -BlinkNearbySearchRadius; OffsetX <= BlinkNearbySearchRadius; ++OffsetX)
		{
			// Skip the player's cell and its direct connecting tiles, since a one-cell blink is what this replaces
			if (FMath::Max(FMath::Abs(OffsetX), FMath::Abs(OffsetY)) < 2)
			{
				continue;
			}

			// Skip the tile in the offset that doesn't exist or is blocked
			const FVector Offset = FVector(OffsetX, OffsetY, 0.f) * FBmrCell::CellSize;
			const FBmrCell CandidateCell = UBmrCellUtilsLibrary::SnapVectorOnLevel(PlayerCell.Location + Offset);
			if (!UBmrCellUtilsLibrary::IsCellExistsOnLevel(CandidateCell) || UBmrCellUtilsLibrary::IsCellBlocked(CandidateCell))
			{
				continue;
			}

			// Only cells closer to the front of the player, and of those the one closest to the given target cell is chosen as the blink target cell
			const double DistanceSquared = FVector::DistSquared2D(CandidateCell.Location, TargetCell.Location);
			if (FVector::DotProduct(Offset, BlinkDirection) > 0.f && DistanceSquared < BestDistanceSquared)
			{
				BestDistanceSquared = DistanceSquared;
				NearbyCell = CandidateCell;
			}
		}
	}

	return NearbyCell;
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

	// Target cell whose location will be passed in for the blink location (if any)
	FBmrCell TargetCell = FBmrCell::InvalidCell;
	
	// Corners are checked first, so squeezing through a corner takes priority over the normal search below
	TargetCell = FindCornerBlinkCell(AvatarPawn, BlinkDirection, PlayerCell);
	const bool bFoundCornerTarget = TargetCell.IsValid();
	
	// Tracks whether an obstacle was encountered while looking for a valid cell. This matters for finding the target cell
	bool bEncounteredObstacle = false;

	// Goes through all cells ahead of the blink direction, only if the corner sweep didn't find a target
	if (!TargetCell.IsValid())
	{
		for (int32 Step = 1; Step <= BlinkTileSearchAmount; ++Step)
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
	
	// If the normal search would only move the player a single cell and not over an obstacle, 
	// blink to the free cell nearby in front of the player that is closest to that target instead.
	// Corner blinks are excluded, since going between corners is a good blink usage
	if (!bFoundCornerTarget && TargetCell.IsValid() 
		&& FVector::Dist2D(TargetCell.Location, PlayerCell.Location) < FBmrCell::CellSize * 1.5f)
	{
		// If a nearby free cell is not found, the single blink range target cell stays
		const FBmrCell NearbyCell = FindNearbyBlinkCell(PlayerCell, BlinkDirection, TargetCell);
		if (NearbyCell.IsValid())
		{
			TargetCell = NearbyCell;
		}
	}
	
	// Don't blink if the target cell is invalid (a valid cell was never found)
	// This only happens if there's no free cell in the entire row/column the player is trying to blink through
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
