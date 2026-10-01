// Copyright (c) Marko Petric & Yevhenii Selivanov

#pragma once

#include "MetaCheatManagerExtension.h"

// UE
#include "HAL/IConsoleManager.h"

#include "SbCheatExtension.generated.h"

/**
 * Extends cheat manager with SkillBlink-related console commands.
 */
UCLASS()
class SKILLBLINKRUNTIME_API USbCheatExtension : public UMetaCheatManagerExtension
{
	GENERATED_BODY()

	/*********************************************************************************************
	 * CVars
	 ********************************************************************************************* */
public:
	/** Override whether corner blinks chain through consecutive corners. */
	static TAutoConsoleVariable<bool> CVarShouldBlinkChainThroughCorners;

	/** Override whether blink should use a tile fallback when blinking only a single tile and not over an obstacle. */
	static TAutoConsoleVariable<bool> CVarShouldBlinkUseTileFallback;

	/** Override the Blink tile fallback search radius, where 1 is the minimum and 8 is the maximum. */
	static TAutoConsoleVariable<int32> CVarBlinkTileFallbackSearchRadius;
};
