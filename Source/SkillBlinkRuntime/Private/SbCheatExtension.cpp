// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "SbCheatExtension.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbCheatExtension)

/*********************************************************************************************
 * CVars
 ********************************************************************************************* */

// Override the impulse strength of the dash ability
TAutoConsoleVariable<int32> USbCheatExtension::CVarBlinkExtraTiles(
	TEXT("Bomber.SkillBlink.SetBlinkExtraTiles"),
	-1.f,
	TEXT("Override blink extra tiles, -1 uses default from data asset"),
	ECVF_Cheat);