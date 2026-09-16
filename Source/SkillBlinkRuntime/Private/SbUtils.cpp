// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "SbUtils.h"

// Sb
#include "Components/SbPlayerStateComponent.h"

// Bomber
#include "GameFramework/BmrPlayerState.h"
#include "UtilityLibraries/BmrBlueprintFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbUtils)

/*********************************************************************************************
 * Object getters
 ********************************************************************************************* */

// Returns Player State Component with a function to broadcast the ability's gameplay event
USbPlayerStateComponent* USbUtils::GetPlayerStateComponent(const UObject* OptionalWorldContext)
{
	const ABmrPlayerState* PlayerState = UBmrBlueprintFunctionLibrary::GetLocalPlayerState(OptionalWorldContext);
	return PlayerState ? PlayerState->FindComponentByClass<USbPlayerStateComponent>() : nullptr;
}
