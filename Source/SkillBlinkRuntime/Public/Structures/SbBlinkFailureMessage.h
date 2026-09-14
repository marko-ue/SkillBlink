// Copyright (c) Marko Petric & Yevhenii Selivanov

#pragma once

#include "SbBlinkFailureMessage.generated.h"

/**
 * Used to pass along information to a widget displaying why a blink failed
 */
USTRUCT(BlueprintType)
struct FSbBlinkFailureMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "[SkillBlink]")
	FText FailureReason = FText::GetEmpty();
};
