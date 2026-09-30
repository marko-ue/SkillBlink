// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "SbCheatExtension.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbCheatExtension)

/*********************************************************************************************
 * CVars
 ********************************************************************************************* */

// Override whether corner blinks chain through consecutive corners
TAutoConsoleVariable<bool> USbCheatExtension::CVarShouldBlinkChainThroughCorners(
	TEXT("Bomber.SkillBlink.ShouldBlinkChainThroughCorners"),
	true,
	TEXT("Set to false to make blink only blink through one corner instead of chaining through all corners"),
	ECVF_Cheat);

// Override whether blink should use a tile fallback when blinking only a single tile and not over an obstacle
TAutoConsoleVariable<bool> USbCheatExtension::CVarShouldBlinkUseTileFallback(
	TEXT("Bomber.SkillBlink.ShouldBlinkUseTileFallback"),
	true,
	TEXT("Set to false to make blink not use tile fallbacks, so you are allowed to blink only 1 tile ahead while not going over an obstacle"),
	ECVF_Cheat);

// Override the Blink tile fallback search radius
TAutoConsoleVariable<int32> USbCheatExtension::CVarBlinkTileFallbackSearchRadius(
	TEXT("Bomber.SkillBlink.BlinkTileFallbackSearchRadius"),
	-1,
	TEXT("Override blink tile fallback search radius, which determines how many tiles around the player the tile fallback should check for. -1 uses default from data asset"),
	ECVF_Cheat);