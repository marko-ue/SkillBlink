// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "SbCheatExtension.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbCheatExtension)

/*********************************************************************************************
 * CVars
 ********************************************************************************************* */

// Override the Blink tile range
TAutoConsoleVariable<int32> USbCheatExtension::CVarBlinkTileRange(
	TEXT("Bomber.SkillBlink.SetBlinkTileRange"),
	-1,
	TEXT("Override blink tile range, -1 uses default from data asset"),
	ECVF_Cheat);

// Override whether the blink range should be infinite (pass through all tiles)
TAutoConsoleVariable<bool> USbCheatExtension::CVarShouldBlinkRangeBeInfinite(
	TEXT("Bomber.SkillBlink.ShouldBlinkRangeBeInfinite"),
	false,
	TEXT("Set to true to make blink range infinite, passing through all tiles"),
	ECVF_Cheat);