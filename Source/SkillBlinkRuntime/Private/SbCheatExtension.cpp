// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "SbCheatExtension.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbCheatExtension)

/*********************************************************************************************
 * CVars
 ********************************************************************************************* */

// Override whether corner blinks chain through consecutive corners
TAutoConsoleVariable<bool> USbCheatExtension::CVarShouldBlinkChainThroughCorners(
	TEXT("Bomber.SkillBlink.CVarShouldBlinkChainThroughCorners"),
	true,
	TEXT("Set to false to make blink only blink through one corner instead of chaining through all corners"),
	ECVF_Cheat);
