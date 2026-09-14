// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "SbGameplayTags.h"

namespace SbGameplayTags
{
	namespace UI
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Widget_BlinkResult, "UI.Widget.SkillBlink.BlinkResult", "Widget tag for the Blink ability Blink result widget");
	} // namespace UI
	
	namespace Event
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BlinkActivated, "Event.SkillBlink.BlinkActivated", "Event that attempts to activate the Blink ability");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BlinkSucceeded, "Event.SkillBlink.BlinkSucceeded", "Event that fires when a Blink succeeded");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BlinkFailed_Occupied, "Event.SkillBlink.BlinkFailed.Occupied", "Event that fires when a Blink fails because of an occupied cell");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BlinkFailed_InvalidCell, "Event.SkillBlink.BlinkFailed.InvalidCell", "Event that fires when a Blink fails because a cell is invalid");
	} // namespace Event
	
	namespace GameplayEffect
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BlinkCooldown, "GameplayEffect.SkillBlink.BlinkCooldown", "Tag for the cooldown of the Blink ability");
	} // namespace GameplayEffect
} // namespace SbGameplayTags