// Copyright (c) Marko Petric & Yevhenii Selivanov

#pragma once

// UE
#include "NativeGameplayTags.h" // UE_DECLARE_GAMEPLAY_TAG_EXTERN

namespace SbGameplayTags
{
	namespace UI
	{
		SKILLBLINKRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Widget_BlinkResult);
	} // namespace UI
	
	namespace Event
	{
		SKILLBLINKRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlinkActivated);
		SKILLBLINKRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlinkSucceeded);
		SKILLBLINKRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlinkFailed_Occupied);
		SKILLBLINKRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlinkFailed_InvalidCell);
	} // namespace Event
	
	namespace GameplayEffect
	{
		SKILLBLINKRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlinkCooldown);
	} // namespace GameplayEffect
	
	namespace GameplayCue
	{
		SKILLBLINKRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlinkSucceeded);
		SKILLBLINKRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlinkFailed);
	} // namespace GameplayCue
} // namespace SbGameplayTags
