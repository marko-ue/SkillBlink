// Copyright (c) Marko Petric & Yevhenii Selivanov

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "SbUtils.generated.h"

/**
 * Static helper functions about SkillBlink.
 */
UCLASS()
class SKILLBLINKRUNTIME_API USbUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/*********************************************************************************************
	 * Object getters
	 ********************************************************************************************* */
public:
	/** Returns the Player State component of SkillBlink. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "[SkillBlink]", DisplayName = "Get Sb Player State Component", meta = (WorldContext = "OptionalWorldContext", CallableWithoutWorldContext))
	static class USbPlayerStateComponent* GetPlayerStateComponent(const UObject* OptionalWorldContext = nullptr);
};
