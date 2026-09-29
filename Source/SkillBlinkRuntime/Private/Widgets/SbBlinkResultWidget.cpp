// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "Widgets/SbBlinkResultWidget.h"

// Sb
#include "SbGameplayTags.h"

// Bomber
#include "Subsystems/GlobalMessageSubsystem.h"

// UE
#include "Components/Image.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbBlinkResultWidget)

/*********************************************************************************************
 * Overrides
 ********************************************************************************************* */

// Called after the underlying slate widget is constructed
void USbBlinkResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind all possible blink outcome events
	UGlobalMessageSubsystem::CallOrStartListeningForGlobalMessage(SbGameplayTags::Event::BlinkFailed_InvalidCell, this, &ThisClass::OnBlinkResult);
	UGlobalMessageSubsystem::CallOrStartListeningForGlobalMessage(SbGameplayTags::Event::BlinkFailed_Occupied, this, &ThisClass::OnBlinkResult);
	UGlobalMessageSubsystem::CallOrStartListeningForGlobalMessage(SbGameplayTags::Event::BlinkSucceeded, this, &ThisClass::OnBlinkResult);

	if (BlinkResultImage)
	{
		BlinkResultImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

// Called when the widget is removed from the viewport
void USbBlinkResultWidget::NativeDestruct()
{
	// Unbind from all events
	UGlobalMessageSubsystem::StopListeningForAllGlobalMessages(this);

	Super::NativeDestruct();
}

/*********************************************************************************************
 * Events
 ********************************************************************************************* */

// Called when the blink ability result is broadcast
// If blink failed, show the blink result image, otherwise hide it
void USbBlinkResultWidget::OnBlinkResult_Implementation(const FGameplayEventData& Payload)
{
	if (Payload.EventTag == SbGameplayTags::Event::BlinkSucceeded)
	{
		BlinkResultImage->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	// Show the blink result image, and starting playing its fade out animation
	BlinkResultImage->SetRenderOpacity(1.f);
	BlinkResultImage->SetVisibility(ESlateVisibility::Visible);
	if (BlinkResultFadeOut)
	{
		PlayAnimation(BlinkResultFadeOut);
	}
}
