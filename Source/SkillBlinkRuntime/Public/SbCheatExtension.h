// Copyright (c) Marko Petric & Yevhenii Selivanov

#pragma once

// Bomber
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
	/** Override the Blink extra tiles, where 0 is the minimum and 8 is the maximum. */
	static TAutoConsoleVariable<int32> CVarBlinkExtraTiles;
	
	/** Override whether the blink range should be infinite (pass through all tiles). */
	static TAutoConsoleVariable<bool> CVarShouldBlinkRangeBeInfinite;
};
