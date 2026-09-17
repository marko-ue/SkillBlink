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
};
